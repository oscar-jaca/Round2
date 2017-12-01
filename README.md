# Round2 implementation

To try out Round2, follow the next steps:

## Prerequisites

To be able to build and run the implementations of the algorithm, the
following conditions must be met:

* The OpenSSL library must be installed. `apt-get install libssl-dev`
  for Debian-based distributions.
  In Mac, an easy way is to use [brew](https://brew.sh), install it with
  `brew install openSSL` and then add it to the CPATH and
  LIBRARY_PATH environment variables:

  ```
  export CPATH=${CPATH+$CPATH:}$HOME/homebrew/opt/openssl/include
  export LIBRARY_PATH=${LIBRARY_PATH+$LIBRARY_PATH:}$HOME/homebrew/opt/openssl/lib
  ```

* The Keccak library must be installed in your system.
  This is done as follows:

  - Linux: Install xsltproc (e.g.  `sudo apt-get install xsltproc`, on
    a Mac it should already be installed)
  - Clone the git repository https://github.com/gvanas/KeccakCodePackage.
  - Build the library, e.g. using `make generic64/libkeccak.a` (for
    alternatives, see the github page)
    * Note that the above step assumes you use a 64-bit environment.
      For a 32-bit environment, the alternative command to run is `make generic32/libkeccak.a`
  - Add the library to your `CPATH` and `LIBRARY_PATH`
  - Alternatively to the above step (if setting the paths does not happen to go smoothly),
    copy the generated Keccak shared library libkeccak.a and the associated headers
    to your system directories. This is done as follows:
    * Copy the Keccak shared library to `/usr/local/lib`
      ```
      cp <KECCAK_INSTALLATION_DIRECTORY>/bin/generic<32/64>/libkeccak.a /usr/local/lib
      ```
    * Copy the associated Keccak headers to /usr/local/include:
      ```
      cp -r <KECCAK_INSTALLATION_DIRECTORY>/bin/generic<32/64>/libkeccak.a.headers /usr/local/include
      ```


## Building the implementations

After installing the prerequisites, you can build the implementations using the
provided `Makefile` inside the [reference](../master/reference)
and [optimized](../master/optimized) directories.

The following make targets are available:

* all:

  Build the implementation, including docs.

* build:

  Build the example applications.

* doc:

  Generates the developer documentation for the implementations.
  Requires doxygen to be installed.

* clean:

  Removes all build artifacts.


## Running the example aplications

The example applications are `kem_example` for Round2.KEM and
`encrypt_example` for Round2.PKE.

The applications take as argument `-aN` to specify the api parameter set to
use, where N is a number between 0 and 29,
(the parameters set are defined in
[`api_to_internal_parameters.h`](../master/reference/src/api_to_internal_parameters.h)),
and `-fN` to specify the fn variant, where N is a number between 0 and 2, as
described in the specification.  Not necessary for parameter sets that make use
of the ring construction.


## Speed Tests

Information about speed tests can be found in the
[speedtest](../master/speedtest/) directory.
