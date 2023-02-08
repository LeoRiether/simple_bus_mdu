.PHONY: cmake run

cmake:
	cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -S . -B build
	mv build/compile_commands.json ./
	cmake --build build

run: cmake
	./build/bus_mdu

