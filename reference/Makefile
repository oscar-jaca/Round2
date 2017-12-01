################################################################################
# Compiler Setup ###############################################################
################################################################################

CC         = gcc

CFLAGSRNG  = -g \
             -I $(srcdir) \
             -O3 -fomit-frame-pointer

# Add -DROUND2_INTERMEDIATE to output intermediate results
CFLAGS 	   = -std=c99 -pedantic -Wall -Wextra -Wconversion -Wcast-qual -Wcast-align \
             $(CFLAGSRNG)

LDLIBS     = -lcrypto -lkeccak -lm

################################################################################
# Dir/File Setup ###############################################################
################################################################################

srcdir    = src
builddir  = build
objdir    = $(builddir)/objs
depdir    = $(builddir)/deps
docdir    = doc

srcs     := $(wildcard $(srcdir)/*.c) $(wildcard $(srcdir)/examples/*.c)
objs     := $(srcs:$(srcdir)/%.c=$(objdir)/%.o)
deps     := $(srcs:$(srcdir)/%.c=$(depdir)/%.d) $(depdir)/createAfixed/createAfixed.d

examples := $(patsubst $(srcdir)/examples/%.c, $(builddir)/%, $(wildcard $(srcdir)/examples/*.c))

################################################################################
# Main Target Rules ############################################################
################################################################################

# Builds the example applications
build: $(examples)

# Builds all, including docs
all: build createAfixed doc

# Builds the createAfixed application
createAfixed: build/createAfixed

# Updates/creates the documentation
# An html version is built in doc/html, a LaTeX version in doc/latex
#
# Requires doxygen to be installed (with dot support)
# The Doxygen configuration is found in Doxyfile
doc:
	@doxygen && echo "Documentation successfully generated"

# Generates a pdf of the latex documentation
# The pdf can be found in doc/latex/refman.pdf
pdf: doc
	@$(MAKE) -C $(docdir)/latex

# latex is an alias for pdf
latex: pdf

# Remove the generated documentation and build files
clean: clean-build clean-doc

# Remove only the generated build files
clean-build:
	@rm -fr $(builddir)

# Remove only the generated dependency files
clean-dep:
	@rm -fr $(depdir)

# Remove only the generated object files
clean-obj:
	@rm -fr $(objdir)

# Remove only the generated documentation
clean-doc:
	@rm -fr $(docdir)

# Above are all "phony" targets
.PHONY: build all doc pdf latex clean clean-obj clean-dep clean-doc clean-all createAfixed

################################################################################
# Object Creation ##############################################################
################################################################################

# Special rule for rng.o
$(objdir)/rng.o: $(srcdir)/rng.c
	@mkdir -p $(dir $@) && \
	$(CC) $(CFLAGSRNG) -c $< -o $@

# Basic rule for standard object files
$(objdir)/%.o: $(srcdir)/%.c
	@mkdir -p $(dir $@) && \
	$(CC) $(CFLAGS) -c $< -o $@

################################################################################
# Executable Creation ##########################################################
################################################################################

build/createAfixed: $(objdir)/parameters.o $(objdir)/randombytes.o $(objdir)/drng.o $(objdir)/misc.o $(objdir)/createAfixed/createAfixed.o
	@$(CC) $(LDFLAGS) $^ $(LOADLIBS) $(LDLIBS) -o $@

$(examples): $(objs)
	@mkdir -p $(builddir) && \
	$(CC) $(LDFLAGS) $(patsubst $(builddir$)/%, $(objdir)/examples/%.o, $@) $(filter-out $(objdir)/examples/%.o, $^) $(LOADLIBS) $(LDLIBS) -o $@


################################################################################
# Dependency Creation and Inclusion ############################################
################################################################################

$(depdir)/%.d: $(srcdir)/%.c Makefile
	@mkdir -p $(dir $@)
	@$(CC) -MM $(CFLAGS) $< | sed 's,\(.*\)\.o[ :]*,$(objdir)/$*.o $@ : Makefile ,g' > $@

ifeq (,$(filter clean% doc latex, $(MAKECMDGOALS)))
-include $(deps)
endif
