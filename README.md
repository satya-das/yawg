# YAWG #
##*Yet Another Wrapper Generator*

*It can generate language binding wrappers for variety of languages for a C/C++ program.*

## Motivation ##
The aim is to create an intelligent wrapper generator tool that does not need interface files as required by other wrapper generators like **SWIG** and **SIP**.

### Goal
 - It should be possible to create wrapper just by parsing third-party headers.
 - Generated wrapper must be readable by human.
 - Generated wrapper should be as close to what a human would produce.
 - There should be variety of wrapper generator components of yawg, e.g. python, JavaScript, Ruby, PHP, etc.
 - There should also be a wrapper generator for C++ binding so that a C++ program can be extended by another C++ program without making it mandatory to build the extension program using same compiler.

### Non-goals
 - Should be able to generate wrapper from reasonably good C/C++ headers. It is not the aim for handling every possible trick that a C/C++ program can use.