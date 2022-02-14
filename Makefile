all:
	cmake -Bbuild -H.
	cmake --build build/ --target all
