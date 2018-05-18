**WARNING**: This is really old, ugly, pre-C++11 code that I had in my computer, but maybe it's useful for somebody. What can I say, this was my first time programming.

# Forbidden Configurations

This is the forbidden configurations program. This program can:
- Compare two configurations to see if one is contained in the other
- Find "what is missing" if a family of configurations is avoided
- Find what asymptotic bound the conjecture predicts for a configuration (i.e. find X(F))
- Find all boundary cases
- Find forb(m,F) exactly for m <= 5
- Find a prediction using local search strategies for forb(m,F) for m <= 8
- Find the inductive children of a configuration

To run, you must first compile. The easiest way to do this is to have the program cmake. In ubuntu: 

```sh
    sudo apt-get install cmake build-essential
```

Then just do (in linux):
```sh
    mkdir build
    cd build
    cmake ..
    make
    ./FConf
```

The program will run. Then follow instructions.

Families of configurations are stored in text files. Many examples are in the directory "Configs". These text files can either be in the directory Configs or in root (where FConf is). Everything except 0's, 1's and line breaks are ignored. For example, the following is a family of two configurations:
```txt
    101
    001
    010

    10  1 dstj2 01
    dd 11 0 1[0,
```

Note that you can put [, or ] or spaces or any other letters and numbers that aren't 0's or 1's and it doesn't make any difference to the program. Just don't use `m` in the middle of a configuration. Hard to explain why (I'm a terrible programmer), but don't use it. Just write configurations in a normal common-sense way.