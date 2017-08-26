
## Compiling Step by Step

This page details how all the magic happened in the [compiling](compiling.md) page for those who care about knowing the
details.  This page explains the steps for Windows, but the same steps apply on other platforms, just using slightly
different tools (like gcc instead msvc, etc).
We will also simplify the steps by focusing on the `darknet` example here, and compiling to run on your host PC.

*Linux instructions are similar except with forward slashes for path separators,
and invoking gcc or clang instead of the MSVC `cl` and `link` commands respectively.*

### Windows 64 bit

First, for this explanation we we need to establish where your ELL root directory is.
If you are in the `build/tutorials/vision/gettingStarted` folder then this should be the root:
```
set ELL_ROOT=..\..\..\..
```    
Next we need to be able to find the ELL compiler which lives here:
```
set PATH=%PATH%;%ELL_ROOT%\build\bin\Release
```
Now run the following command to compile the darknet model to LLVM intermediate representation (IR) and to generate the Python wrapper code for the model:
```
compile -imap darknetReference.map -cfn predict -cmn darknetReference --ir --swig
```
*Note:* this may take a minute or more.

Next we need to run SWIG to generate the C++ Python wrappers.  If you installed SWIG manually then it is probably
already in your PATH, if not then you will find it here (on Windows):
```
set PATH=%PATH%;%ELL_ROOT%\external\swigwintools.3.0.12\tools\swigwin-3.0.12
```
Then run:
```
swig -python -modern -c++ -Fmicrosoft -py3 -outdir . -c++ -I%ELL_ROOT%\interfaces\common\include -I%ELL_ROOT%\interfaces\common -I%ELL_ROOT%\libraries\emitters\include -o _darknetReferencePYTHON_wrap.cxx darknetReference.i
```
This should be quick.  Next we run `llc` to compile the IR language generated by compile step above
into a .obj linkable module.  This means we need to be able to find llc:
```
set PATH=%PATH%;%ELL_ROOT%\external\LLVMNativeWindowsLibs.x64.3.9.1\build\native\tools\
```
Then run this:
```
llc -filetype=obj darknetReference.ll -march x86-64
```
*Note:* the machine architecture we chose there means you need to be running it on 64 bit Windows.
`llc` supports many other targets, including what we need to run on Raspberry Pi.

Now we can build the Python module using the MSVC compiler.  This means you need to open the
Visual Studio 2015 X64 Native Tools Command Prompt, to ensure you are building a 64 bit executable.

The compiler and linker will need to be able to find your Python 3.6 SDK.  If you type `where python`
you will see where the Python executable lives, then set the following variable to point to that location:
```
set PY_ROOT=d:\Continuum\Anaconda2\envs\py36
```
In my case I was using Anaconda.  Now you can run the Visual Studio C++ compiler to compile the Python wrapper:
```
cl /I%PY_ROOT%\include /I%ELL_ROOT%\interfaces\common\include\ /I%ELL_ROOT%\libraries\emitters\include\ /c /EHsc /MD _darknetReferencePYTHON_wrap.cxx
```
And we finish up with the Visual Studio C++ linker to produce the Python-loadable module.
The linker may also need to find the libOpenBLAS library which lives.   OpenBLAS is a library that is optimized for specific types of CPUs, right down to the CPU model number.  So you need to pick the right processor type.
For Intel, we support either haswell or sandybridge.  Note that Ivy Bridge is compatible with Sandy Bridge, and Broadwell is compatible with Haswell.  When you ran `cmake` for ELL part of the cmake output will tell you what processor family you have, you should see some output like this:
```
-- Processor family: 6, model: 79
-- Using OpenBLAS compiled for haswell
```
So this means I need to use this version of OpenBLAS:
``` 
set OPENBLAS=%ELL_ROOT%\external\OpenBLASLibs.0.2.19.3\build\native\x64\haswell\lib
```
Then run the linker:
```
link darknetPYTHON_wrap.obj darknetReference.obj %PY_ROOT%\libs\python35.lib %OPENBLAS%\libopenblas.dll.a /MACHINE:x64 /SUBSYSTEM:WINDOWS /DLL /DEBUG:FULL /PDB:_darknetReference.pdb /OUT:_darknetReference.pyd
```
#### Running the Compiled Model

Ok, now that _darknetReference.pyd exists, we can load it up into Python and see if it works. From your Anaconda Python 3.6 environment run this:
```
python compiledDarknetDemo.py
```
If this fails to load, it could be because it is failing to find the OpenBLAS library, so do this:
```
set PATH=%PATH%;%OPENBLAS%
```
(using the OPENBLAS variable we defined earlier on this page) then try again.

One thing you should notice is that this compiled version of darknet loads instantly and runs quickly.