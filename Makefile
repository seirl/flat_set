TARGET=flatset
SRCDIR=src
FILES=
LIBNAME=$(LIBDIR)$(TARGET)

include Makefile.rules

SRC=$(addprefix $(SRCDIR)/, $(FILES))
OBJS=$(SRC:.cc=.o) src/main.o
OBJSTEST=$(SRC:.cc=.test.o) src/main.test.o
OBJSPIC=$(SRC:.cc=.o.fPIC) src/flatset_python.o.fPIC
DEPS=$(OBJS:.o=.d)
CXX?= g++
CXXFLAGS+=-std=c++11
CPPFLAGS+=-MMD -I$(INCLUDES)
LIBDIR=
LDFLAGS=$(shell python-config --libs) -lboost_python
ifneq ($(LIBDIR), )
  LDFLAGS+=-L$(LIBDIR)
endif
INCLUDES=$(addprefix $(SRCDIR)/, include) $(shell python-config --includes)
STATICLIB=$(LIBNAME).a
DYNAMICLIB=$(LIBNAME).so
ARCHIVEPREFIX=tolmer_v-$(TARGET)
ARCHIVENAME=$(ARCHIVEPREFIX).tar.bz2


all: dynamiclib

debug: CXXFLAGS += -g

debug: all

check: CPPFLAGS += -UNDEBUG
check: TARGET = check

check: $(OBJSTEST)
	@$(CXX) $^ $(LDFLAGS) -o $@
	./check

exec: $(TARGET)

bench: CPPFLAGS += -DNDEBUG

bench: exec

export:
	git archive master --prefix=$(ARCHIVEPREFIX)/ | bzip2 > $(ARCHIVENAME)

$(TARGET): $(OBJS)
	@echo LD $@ ...
	@$(CXX) $^ $(LDFLAGS) -o $@

staticlib: $(STATICLIB)

$(STATICLIB): $(OBJS)
	@echo static lib: $@ ...
	@ar csr $@ $^

dynamiclib: $(DYNAMICLIB)

$(DYNAMICLIB): $(OBJSPIC)
	@echo dynamic lib: $@ ...
	@$(CXX) $(LDFLAGS) --shared -o $@ $^

%.o.fPIC:CXXFLAGS+=-fPIC

%.o.fPIC: %.cc
	@echo FPIC $@ ...
	$(COMPILE.cc) $(OUTPUT_OPTION) $<

%.test.o: %.cc
	@echo CC $@ ...
	@$(COMPILE.cc) $(CPPFLAGS) $(OUTPUT_OPTION) $<

%.o: %.cc
	@echo CC $@ ...
	@$(COMPILE.cc) $(CPPFLAGS) $(OUTPUT_OPTION) $<

clean:
	@echo Removing files ...
	@$(RM) $(OBJS) $(OBJSPIC) $(OBJSTEST) $(TARGET) $(STATICLIB) $(DYNAMICLIB) $(DEPS)
	@$(RM) $(ARCHIVENAME) $(PICDEPS)

.PHONY: all clean staticlib dynamiclib exec check bench

-include $(DEPS)
