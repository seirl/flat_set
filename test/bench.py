from __future__ import print_function
import random
import time
from flatset import flatset

s = set()
t0 = time.time()
for i in range(100000):
    s.add(i)
print('set ordered:', time.time() - t0)

s = flatset()
t0 = time.time()
for i in range(100000):
    s.add((i))
print('flatset ordered:', time.time() - t0)


s = set()
t0 = time.time()
for i in range(100000):
    s.add((random.randint(0, 100000)))
print('set random:', time.time() - t0)

s = flatset()
t0 = time.time()
for i in range(100000):
    s.add((random.randint(0, 100000)))
print('flatset random:', time.time() - t0)
