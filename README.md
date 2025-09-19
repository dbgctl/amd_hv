# amd_hv

A very basic type 2 hypervisor base for Windows 10/11, with implemented usermode communication via instruction interception.

## Features

* Simple codebase to start hypervising with for beginners.

* Easily expandable, with heavily-commented code to explain how each part works.

* Allows custom instruction and exception interception out-of-the-box, fully customizable host context, and room for further expansion.

* Safe loading and unloading with compatability and integrity check, as well as comprehensive cleanup.

* Fully typed out AMD-SVM structures that allow for learning directly from the source.

## Usage

This hypervisor is intented be used as a learning case, studying how SVM instructions work, however could also serve as base for developing a more complex, specialized hypervisor.
Protecting the host from the guest isnt included the scope of this project, but can be expanded on as an excercise for learners.

## Sources

Most parts have been made in reference to AMD's latest manual, namely AMD64 Architecture Programmer’s Manual Volume 2: System Programming Publication no. 24593 Revision 3.43.
Unless comments state otherwise, assume that was the source.
Other information sources include:
ia32-doc - Saving time on typing out structures that are the same across intel and amd cpus.
OSDev wiki - Exception vector with error-codes.

## Links to sources

* AMD manual
https://docs.amd.com/v/u/en-US/24593_3.43

* ia32
https://github.com/ia32-doc/ia32-doc

* OSDev
https://wiki.osdev.org