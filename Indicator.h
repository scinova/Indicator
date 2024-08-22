#include <stdint.h>

class Indicator {
	public:
		Indicator();
		void begin(int front_total_pixels, int rear_num_pixels, int rear_total_pixels, int rear_side_pixels);
		uint32_t getFrontPixel(int index);
		uint32_t getRearPixel(int index);
		void visibility(bool enable);
		void visibility();
		void lowbeam(bool enable);
		void lowbeam();
		void highbeam(bool enable);
		void highbeam();
		void left(bool enable);
		void left();
		void right(bool enable);
		void right();
		void hazard(bool enable);
		void hazard();
		//void reverse(bool enable);
		//void brake(bool enable);
		void loop();
		struct {
			bool visibility;
			bool reverse;
			bool brake;
			bool hazard;
			bool left;
			bool right;
			bool highbeam;
			bool lowbeam;
		} status;
	private:
		int front_total_pixels;
		int front_side_pixels;
		int rear_total_pixels;
		int rear_side_pixels;
};
