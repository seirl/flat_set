TARGET=flatset
SRCDIR=src
FILES=
LIBNAME=$(LIBDIR)$(addprefix lib, $(TARGET))

include Makefile.rules

SRC=$(addprefix $(SRCDIR)/, $(FILES))
OBJS=$(SRC:.cc=.o) src/main.o
OBJSPIC=$(SRC:.cc=.o.fPIC) src/flatset_python.o.fPIC
DEPS=$(OBJS:.o=.d)
CXX?= g++
CXXFLAGS+=-std=c++11
CPPFLAGS=-MMD -I$(INCLUDES)
LIBDIR=
ifneq ($(LIBDIR), )
  LDFLAGS=-L$(LIBDIR) -lboost_python -lpython2.7
endif
INCLUDES=$(addprefix $(SRCDIR)/, include)
STATICLIB=$(LIBNAME).a
DYNAMICLIB=$(LIBNAME).so
ARCHIVEPREFIX=tolmer_v-$(TARGET)
ARCHIVENAME=$(ARCHIVEPREFIX).tar.bz2


all: exec

debug: CFLAGS += -g

debug: all

check:
	@cd test && python testsuite.py

exec: $(TARGET)

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
	@$(CXX) --shared -o $@ $^

%.o.fPIC:CXXFLAGS+=-fPIC

%.o.fPIC: %.cc
	@echo FPIC $@ ...
	$(COMPILE.cc) $(OUTPUT_OPTION) $<


%.o: %.cc
	@echo CC $@ ...
	@$(COMPILE.cc) $(OUTPUT_OPTION) $<

clean:
	@echo Removing files ...
	@$(RM) $(OBJS) $(OBJSPIC) $(TARGET) $(STATICLIB) $(DYNAMICLIB) $(DEPS)
	@$(RM) $(ARCHIVENAME) $(PICDEPS)

.PHONY: all clean staticlib dynamiclib exec check

-include $(DEPS)
