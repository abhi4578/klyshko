# Compiler flags
CFLAGS = -Wall -Wextra

# Gramine log level based on debug or production mode
ifeq ($(DEBUG),1)
GRAMINE_LOG_LEVEL = debug
CFLAGS += -g
else
GRAMINE_LOG_LEVEL = error
CFLAGS += -O3
endif

# Targets that should always run
.PHONY: all clean distclean check

# Main target to build everything
all: file_2 file_2.manifest
ifeq ($(SGX),1)
all: file_2.manifest.sgx file_2.sig
endif

# Rule for compiling the C++ program
file_2: file_2.o
	$(CXX) $(CFLAGS) -o $@ $^

# Rule for compiling object file from the source
file_2.o: file_2.cpp
	$(CXX) $(CFLAGS) -c -o $@ $<

# Rule for generating the manifest file from the template
file_2.manifest: file_2.manifest.template file_2
	gramine-manifest \
		-Dlog_level=$(GRAMINE_LOG_LEVEL) \
		$< $@
	gramine-manifest-check $@

# Workaround for signing Gramine manifest to prevent concurrency issues
file_2.sig file_2.manifest.sgx: sgx_sign
	@:

.INTERMEDIATE: sgx_sign
sgx_sign: file_2.manifest
	gramine-sgx-sign \
		--manifest $< \
		--output $<.sgx
	gramine-manifest-check $<.sgx

# Determine whether to run in SGX mode or direct mode
ifeq ($(SGX),)
GRAMINE = gramine-direct
else
GRAMINE = gramine-sgx
endif

# Rule to test the application after building it
check: all
	$(GRAMINE) file_2 > OUTPUT
	# Optionally, you can check if output is as expected
	# echo "Expected Output" | diff OUTPUT -
	@echo "[ Success ]"

# Rule for cleaning up generated files
clean:
	$(RM) *.token *.sig *.manifest.sgx *.manifest file_2.o file_2 OUTPUT

# Rule for deep cleaning, including the compiled executable
distclean: clean
	$(RM) file_2
