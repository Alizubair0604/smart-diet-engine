@echo off
echo Compiling Smart Diet Engine...
gcc kirtan_gui.c ali_data.c math_engine.c libraylib.a -o smart_diet_app.exe -lopengl32 -lgdi32 -lwinmm
if %errorlevel% neq 0 (
    echo Compilation failed!
    exit /b %errorlevel%
)
echo Build finished successfully! Running app...
.\smart_diet_app.exe