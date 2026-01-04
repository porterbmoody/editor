#include <cstdlib>

int main() {
	system(
		"ffmpeg -loop 1 -i future.jpg -i warp3.wav "
		"-c:v libx264 -tune stillimage "
		"-c:a copy -shortest -pix_fmt yuv420p output.mp4"
	);
}


// C:/msys64/mingw64/bin/c++.exe vid_gen.cpp -o vid_gen.exe