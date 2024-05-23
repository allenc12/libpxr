SRCDIR := libpxr

STATICLIB := $(SRCDIR).a
SHAREDLIB := $(SRCDIR).dylib

INCLUDES = -I$(SRCDIR)
CFILES := pxr_open.c

SRCS := $(addprefix $(SRCDIR)/, $(CFILES))
OBJS := $(SRCS:.c=.o)

TESTDIR := tests
TESTSRC := test_read.c
TESTS := $(addprefix $(TESTDIR)/, $(TESTSRC))
TESTOBJ := $(TESTS:.c=.o)

# CCFLAGS = $(CFLAGS) $(CCFLAGS) -std=c17 -g -O0
CCFLAGS = $(CFLAGS) -std=c17 -g -O0
# CPPFLAGS = $(CPPFLAGS) -Wall -Wextra -pedantic $(INCLUDES)
CPPFLAGS = -Wall -Wextra -pedantic $(INCLUDES)
LDLIBS = -lpxr
LDFLAGS = -L. $(LDLIBS) -Wl,-rpath,$(eval `pwd`)

.PHONY: all shared static clean distclean test
all: static

static: $(STATICLIB) $(OBJS)
	$(AR) -rcs $(STATICLIB) $(OBJS)

$(STATICLIB): $(OBJS)

shared: $(SHAREDLIB)
	$(CC) -shared -o $@ $(OBJS)

$(SHAREDLIB): $(OBJS)
	$(eval CFLAGS=-fPIC)

%.o: %.c
	$(CC) $(CCFLAGS) $(CPPFLAGS) -c -o $@ $<

test: static
	$(CC) $(CCFLAGS) $(STATICLIB) tests/test_read.c -o a.out

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) $(STATICLIB) $(SHAREDLIB)