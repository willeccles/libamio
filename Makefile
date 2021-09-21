SRCDIR = src
INCDIR = include
BUILDDIR = build

SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%.o,$(SRC))
DEP = $(addsuffix .d,$(OBJ))

STATICLIB = $(BUILDDIR)/libamio.a
SHAREDLIB = $(BUILDDIR)/libamio.so

# allow for cross compilation
_CC := $(CROSS_COMPILE)$(CC)
_LD := $(CROSS_COMPILE)$(LD)
_AR := $(CROSS_COMPILE)ar

CFLAGS += -O3 -std=c99 -fPIC -Wall -Werror -pedantic
CPPFLAGS += -D_XOPEN_SOURCE=700 -I$(INCDIR) -MMD -MP
LDFLAGS += -fwhole-program -static

.PHONY: all shared static clean docs

all: shared static

shared: $(SHAREDLIB)

static: $(STATICLIB)

$(STATICLIB): $(OBJ)
	$(_AR) -crs $@ $?

$(SHAREDLIB): $(OBJ)
	$(_LD) $(LDFLAGS) -shared -o $@ $?

-include $(DEP)

$(BUILDDIR)/%.c.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(_CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $?

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	$(RM) -r $(BUILDDIR)

docs:
	doxygen Doxyfile
