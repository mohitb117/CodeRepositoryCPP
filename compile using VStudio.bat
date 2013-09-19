@Echo off
echo "==============================================================="
echo "Rebuilding and Compiling Solution Project 4:-"
echo "==============================================================="
path=%path%;C:\WINDOWS\Microsoft.NET\Framework\v3.5\
path=%path%;C:\Program Files (x86)\Microsoft Visual Studio 11.0\SDK\v3.5\Bin

path=%path%;C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\Bin
path=%path%;C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include
path=%path%;c:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\PlatformSDK\Include
path=%path%;c:\Program Files (x86)\Microsoft Visual Studio 11.0\Common7\Tools\Bin
path=%path%;C:\Program Files (x86)\Microsoft Visual Studio 11.0\Common7\IDE

set include=%include%;c:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\PlatformSDK\Include
set lib=%lib%;c:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\PlatformSDK\lib

path=%path%;D:\VS2012\Microsoft Visual Studio 11.0\Common7\IDE

path=%path%;D:\VS2012\Common7\IDE

devenv.exe RemoteCodeStore.sln /rebuild

echo "Rebuild Finished"

pause