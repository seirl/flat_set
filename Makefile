TARGET=flatset
SRCDIR=src
FILES=
LIBNAME=$(LIBDIR)$(addprefix lib, $(TARGET))

SRC=$(addprefix $(SRCDIR)/, $(FILES))
OBJS=$(SRC:.c=.o) src/main.o
OBJSPIC=$(SRC:.c=.o.fPIC)
DEPS=$(OBJS:.o=.d)
PICDEPS=$(OBJSPIC:.fPIC=.d)
CC=g++
CXXFLAGS=-Wall -Wextra -std=c++11 -g
CPPFLAGS=-MMD -I$(INCLUDES)
LIBDIR=
ifneq ($(LIBDIR), )
  LDFLAGS=-L$(LIBDIR)
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
	cd test && ./testsuite.py

exec: $(TARGET)

export:
	git archive master --prefix=$(ARCHIVEPREFIX)/ | bzip2 > $(ARCHIVENAME)

$(TARGET): $(OBJS)
	@echo LD $@ ...
	$(CC) $^ $(LDFLAGS) -o $@

staticlib: $(STATICLIB)

$(STATICLIB): $(OBJS)
	@echo static lib: $@ ...
	@ar csr $@ $^

dynamiclib: $(DYNAMICLIB)

$(DYNAMICLIB): $(OBJSPIC)
	@echo dynamic lib: $@ ...
	@$(CC) --shared -o $@ $^

%.o.fPIC:CFLAGS+=-fPIC

%.o.fPIC: %.c
	@echo FPIC $@ ...
	@$(COMPILE.c) $(OUTPUT_OPTION) $<

%.o: %.c
	@echo CC $@ ...
	@$(COMPILE.c) $(OUTPUT_OPTION) $<

clean:
	@echo Removing files ...
	@$(RM) $(OBJS) $(OBJSPIC) $(TARGET) $(STATICLIB) $(DYNAMICLIB) $(DEPS)
	@$(RM) $(ARCHIVENAME) $(PICDEPS)

.PHONY: all clean staticlib dynamiclib exec check

-include $(DEPS)
-include $(PICDEPS)
