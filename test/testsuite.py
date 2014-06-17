#! env python


# This testsuite is designed in the following fashion :
# A test is considered present if the folder with its test number is present
# The tests are executed in increasing order, starting at 1
# This testsuite requires a test.program file, with the basic command line

# The following files are created:
# ret: return code
# out: standard output
# err: standard error

# The following are tested, if present, in this order:
# ret.ref
# out.ref
# err.ref
# check: script running extra checks, returning 0 on success

# Other files:
# test: Arguments
# in: standard input
# pre: script to run before the test
# post: script to run after the test
# fail: script to run in case of test failure

# Global files, in the root folder:
# test.program: program name
# test.pre: script run before any test is run
# test.post: scritp run after displaying the result of the testsuite

import os, sys
import subprocess
import filecmp
import difflib

COLOR_RED="\033[1;91m"
COLOR_GREEN="\033[1;92m"
COLOR_BLUE="\033[1;94m"
COLOR_RESET="\033[0m"

passed = 0
failed = []

def splitq(seq, sep=None, quote='"\'') :
    if not seq:
        yield ""
    else:
        lsep = len(sep) if sep is not None else 1
        start = index = 0
        while 0 <= index < len(seq):
            c = seq[index]
            #print index, c
            if (sep and seq[index:].startswith(sep)) \
                 or (sep is None and c.isspace()):
                if seq[start] in quote:
                    yield seq[start + 1:index - 1]
                else:
                    yield seq[start:index]
                #pass multiple separators as single one
                if sep is None:
                    index = len(seq) - len(seq[index:].lstrip())
                    #if index < len(seq):
                    #    print(repr(seq[index]),index)
                else:
                    while (sep and seq[index:].startswith(sep)):
                        index = index + lsep
                start = index
            elif c in quote:
                index += 1
                p, index = index, seq.find(c,index) + 1
                if not index:
                    raise IndexError('Unmatched quote %r\n%i:%s' % (c, p,
                        seq[:p]))
            else:
                index += 1
        if seq[start:]:
            yield seq[start:]

def getargs(seq):
    return [s for s in splitq(seq)]

def isNumber(s):
    try:
        int(s)
        return True
    except ValueError:
        return False

def getDiff(ref, out, indent):
    d = difflib.Differ ()
    message = ""
    for line in (list(d.compare(open(ref).readlines(),
        open(out).readlines()))):
        message += indent + line
    return message

def fail(n):
    print COLOR_RED + "KO" + COLOR_RESET
    global failed
    failed.append(n)

def exists(path):
    return os.path.isfile(path)

def getLog(number):
    prefix = str(number) + "/"
    log = ""
    if exists(prefix + "title"):
        log += open(prefix + "title").readline() + "\n"
    else:
        log += "Test " + str(number) + ":\n\n"

    log += "=============\n"

    log += "return code: " + open(prefix + "ret").readline() + "\n"
    if exists(prefix + "ret.ref"):
        log += "reference: " + open(prefix + "ret.ref").readline() + "\n"

    log += "=============\n"

    log += "standard out:\n" + open(prefix + "out").readline() + "\n"
    if exists(prefix + "out.ref"):
        log += "reference:\n" + open(prefix + "out.ref").readline() + "\n"
        if not filecmp.cmp(prefix + "out", prefix + "out.ref"):
            log += ("diff:\n" + getDiff(prefix + "out.ref", prefix + "out", "")
                    + "\n")

    log += "=============\n"

    log += "standard err:\n" + open(prefix + "err").readline() + "\n"
    if exists(prefix + "err.ref"):
        log += "reference:\n" + open(prefix + "err.ref").readline() + "\n"
        if not filecmp.cmp(prefix + "err", prefix + "err.ref"):
            log += ("diff:\n" + getDiff(prefix + "err.ref", prefix + "err", "")
                    + "\n")
    return log

def checkDiff(name, message, number):
    if exists(name + ".ref"):
        if not filecmp.cmp(name, name + ".ref"):
            fail(number)
            print "  " + message + ":"
            print getDiff(name + ".ref", name, "    "),
            return 1
    return 0

def runCheck(number):
    if exists("check"):
        ret = subprocess.call(["./" + prefix + "check"])
        if ret:
            fail(number)
            print "  extra tests failed"
        return ret
    else:
        return 0

CLEANUP=True

def runTest(number):
    prefix = str(number) + "/"
    print "%3d: " % number,
    title = ""
    if exists(prefix + "title"):
        title = open(prefix + "title").readline()[:-1] + ": "
    print "%-30s" % title,
    if exists(prefix + "pre"):
        os.system("./" + prefix + "pre")

    filein = None
    if exists(prefix + "in"):
        filein = open(prefix + "in", "r")

    args = []
    if exists(prefix + "program"):
        args = getargs(open(prefix + "program", "r").read())
    else:
        args = program[:]
    if exists(prefix + "test"):
        args.extend(getargs(open(prefix + "test").readline()))
    fileout = open(prefix + "out", "w")
    fileerr = open(prefix + "err", "w")
    retcode = subprocess.call(args, stdin = filein,
            stdout = fileout, stderr = fileerr)

    open(prefix + "ret", "w").write(str(retcode) + "\n")

    if (checkDiff(prefix + "ret", "return code different", number) or
        checkDiff(prefix + "out", "standard output different", number) or
        checkDiff(prefix + "err", "standard error different", number) or
        runCheck(number)):
        open(prefix + "log", "w").write(getLog(number))
        if exists(prefix + "fail"):
            os.system("./" + prefix + "fail")
        return
    print COLOR_GREEN + "OK" + COLOR_RESET
    global passed
    passed += 1

    if exists(prefix + "post"):
        os.system("./" + prefix + "post")

    if CLEANUP:
        os.remove(prefix + "out")
        os.remove(prefix + "ret")
        os.remove(prefix + "err")

def cleanFiles():
    i = 1;
    while os.path.isdir(str(i)):
      prefix = str(i) + "/"
      if os.path.isfile(prefix + "out"):
        os.remove(prefix + "out")
      if os.path.isfile(prefix + "ret"):
        os.remove(prefix + "ret")
      if os.path.isfile(prefix + "err"):
        os.remove(prefix + "err")
      i += 1

n = 0
TESTLIST = False
RECHECK = False
testlist = []

if not os.path.isfile("test.program"):
    print COLOR_RED + "Missing test.program file!"
    sys.exit(1)

program = getargs(open("test.program", "r").readline())

for i in range(1, len(sys.argv)):
    if sys.argv[i][0] == '-':
        for j in range(1, len(sys.argv[i])):
            if sys.argv[i][j] == 'r':
                RECHECK=True
            elif sys.argv[i][j] == 'n':
                CLEANUP=False

if len(sys.argv) > 1:
    if sys.argv[1] == "clean":
        cleanFiles()
        sys.exit(0)
    elif sys.argv[1] == "recheck":
        RECHECK=True
    else:
        for i in range(1, len(sys.argv)):
            if isNumber(sys.argv[i]):
                TESTLIST=True
                testlist.append(int(sys.argv[i]))

if os.path.isfile("test.pre"):
  os.system("./test.pre");

if TESTLIST:
    for i in testlist:
        if RECHECK and not os.path.isfile(str(i) + "/out"):
            continue
        runTest(i)
else:
    while os.path.isdir(str(n + 1)):
        n += 1
        if RECHECK and not os.path.isfile(str(n) + "/out"):
            continue
        runTest(n)

if failed:
    print (COLOR_RED + "Passed " + str(passed) + "/" + str(passed + len(failed))
            + " tests." + COLOR_RESET)
    print "Tests failed:",
    logtest = open("testsuite.log", "w")
    for t in failed:
        print str(t) + " ",
        logtest.write(open(str(t) + "/log", "r").read() +
            "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n")

    print ""
else:
    print (COLOR_GREEN + "All " + str(passed) + " tests passed. Hurray!"
            + COLOR_RESET)

if os.path.isfile("test.post"):
  os.system("./test.post");
