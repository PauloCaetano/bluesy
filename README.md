bluesy - Not exactly a lib (yet)
================================

## Description

Bluesy is a group of reusable components I'm creating as I develop in C++.
There is no grandiose unified vision, I'll be adding to it as the need arises.

## System Requirements

- gcc, mingw on Windows (tested with 4.7+)
- ICU (tested with 51+)
- Boost (tested with 1.53+)
    - Exception
    - Locale
    - Program Options
    - Test

## Module List

### App Configuration

- prog_options

 Defines a class template that deals with the generic part of using Boost
Program Options. The class template is then parametrized with another class,
which will take care of its app's specific options.

### MS Windows

- win_console_out

 The functions defined in this module output messages on an MS Windows console,
performing the necessary conversions to correctly display non-ASCII characters.