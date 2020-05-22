
$directory = "C:\dev\eclipse\WordWrap"
$executable = "WordWrap.exe"
$compiler = "g++"
$args = "-std=c++11 -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0"
$build = "Debug"

Import-Module Projects

$builder = New-CppProjectBuilder `
	-ProjectDir $directory `
	-NewExecutable $executable `
	-Compiler $compiler `
	-CompilerArgs $args `
	-BuildFolder $build
	
$builder
"`n"
Clean-CppProject $builder
Build-CppProject $builder
