TARGET=flatset
SRCDIR=src
FILES=
LIBNAME=$(LIBDIR)$(addprefix lib, $(TARGET))

include Makefile.rules

SRC=$(addprefix $(SRCDIR)/, $(FILES))
OBJS=$(SRC:.c=.o) src/main.o
DEPS=$(OBJS:.o=.d)
CXX?= g++
CXXFLAGS+=-std=c++11
CPPFLAGS=-MMD -I$(INCLUDES)
INCLUDES=$(addprefix $(SRCDIR)/, include)
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

%.o: %.cc
	@echo CC $@ ...
	@$(COMPILE.cc) $(OUTPUT_OPTION) $<

clean:
	@echo Removing files ...
	@$(RM) $(OBJS) $(OBJSPIC) $(TARGET) $(STATICLIB) $(DYNAMICLIB) $(DEPS)
	@$(RM) $(ARCHIVENAME) $(PICDEPS)

.PHONY: all clean staticlib dynamiclib exec check

-include $(DEPS)
