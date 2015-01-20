@echo off
REM Debug Options
REM set CommonCompilerFlags=-MTd -nologo -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -FC -Z7 
REM set CommonLinkerFlags= -incremental:no -opt:ref

REM Release Options
set CommonCompilerFlags=-MTd -nologo -Oxs /GL
set CommonLinkerFlags= -incremental:no -opt:ref

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

REM 32-bit build
REM cl %CommonCompilerFlags% ..\code\punch.cpp /link -subsystem:windows,5.1 %CommonLinkerFlags%

REM 64-bit build
del *.pdb > NUL 2> NUL
cl %CommonCompilerFlags% ..\code\punch.cpp -Fmpunch.map  /link %CommonLinkerFlags%
 
popd
