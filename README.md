# Alogir - Algorithms over containers

Alogir is a collection of algorithm which can be applied over
container libraries. The targeted container libraries are:

* Slinky
* Gromer
* Mapper
* Framer
* Ringer


The number of algorithms will be growing. Currenty there is:

* ag_hash - Fast hashing, used in Mapper.

* ag_heap - Binary heap sorting based on Gromer.


## Alogir API documentation

See Doxygen documentation. Documentation can be created with:

    shell> doxygen .doxygen


## Examples

All functions and their use is visible in tests. Please refer `test`
directory for testcases.


## Building

Ceedling based flow is in use:

    shell> ceedling

Testing:

    shell> ceedling test:all

User defines can be placed into `project.yml`. Please refer to
Ceedling documentation for details.


## Ceedling

Alogir uses Ceedling for building and testing. Standard Ceedling files
are not in GIT. These can be added by executing:

    shell> ceedling new alogir

in the directory above Alogir. Ceedling prompts for file
overwrites. You should answer NO in order to use the customized files.
