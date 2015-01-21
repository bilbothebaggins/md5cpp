# What's where

* `src/md5/md5.sln` - VS2013 solution containing all the stuff.

# md5cpp

This is simply a C++ implementation of the MD5 hashing algorithm.

Existing `C++` implementations I have found were unfortunately unclear wrt. their
 licensing status, so I decided to find a unencumbered one and polish up that.

I only found a suitable [C implementation](https://www.fourmilab.ch/md5/) so I 
decided to just go, wrap that and polish up the C code to C++ code to my liking.

Because Windows is my habitat, it's implemented as a Windows DLL, but ripping out 
the code should be a no brainer.

The projects are VS2013Express, because this is written in my free time and that's 
what I have installed already.
 
#md5cli

A .NET C++/CLI wrapper for my `md5cpp`.

Actually the *Raison d'être* for that whole stuff here (also the C++ version).

Found the existing .NET implementation lacking, needed a replacement, didn't find something to my liking, put together my own.

Should be easy to use, now also implements ICryptoTransform.
