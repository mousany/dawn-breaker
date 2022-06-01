Get-ChildItem .\build | Remove-Item -Recurse

cd .\build
cmake .. -G "Visual Studio 16 2019"

Import-Module -Name 'D:\\Microsoft Visual Studio\\BuildTools\\Common7\\Tools\\Microsoft.VisualStudio.DevShell.dll'
Enter-VsDevShell ab95e860

msbuild /p:Configuration=Release Dawnbreaker.sln

if($?){
    echo "Build Successful"
    .\bin\Release\Dawnbreaker.exe
}
else{
    echo "Build Failed"
}

cd ..