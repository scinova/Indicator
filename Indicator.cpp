#include "Indicator.h"
#include <Arduino.h>

#define AMBER 0x00FFBF00
#define WHITE 0x00FFFFFF
#define BLACK 0x00000000
#define RED   0x00FF0000
#define DARKRED 0x00CC0000

Indicator::Indicator() {
}

void Indicator::begin(int front_total_pixels_, int front_side_pixels_,
		int rear_total_pixels_, int rear_side_pixels_) {
	front_total_pixels = front_total_pixels_;
	front_side_pixels = front_side_pixels_;
	rear_total_pixels = rear_total_pixels_;
	rear_side_pixels = rear_side_pixels_;
}

void Indicator::visibility() {
	visibility(!status.visibility);
}

void Indicator::visibility(bool enable) {
	if (enable && !status.visibility)
		status.visibility = true;
	else if (!enable && status.visibility && !status.highbeam && !status.lowbeam)
		status.visibility = false;
}

void Indicator::lowbeam() {
	lowbeam(!status.lowbeam);
}

void Indicator::lowbeam(bool enable) {
	if (enable && !status.lowbeam)
		status.lowbeam = true;
	else if (!enable && status.lowbeam && !status.highbeam)
		status.lowbeam = false;
}

void Indicator::highbeam() {
	highbeam(!status.highbeam);
}

void Indicator::highbeam(bool enable) {
	if (enable && !status.highbeam)
		status.highbeam = true;
	else if (!enable && status.highbeam)
		status.highbeam = false;
}

void Indicator::hazard() {
	hazard(!status.hazard);
}

void Indicator::hazard(bool enable) {
	if (enable && !status.hazard) {
		status.left = false;
		status.right = false;
		status.hazard = true;
	} else if (!enable && status.hazard)
		status.hazard = false;
}

void Indicator::left() {
	left(!status.left);
}

void Indicator::left(bool enable) {
	if (status.hazard)
		return;
	if (enable) {
		if (status.right)
			status.right = false;
		status.left = true;
	} else
		status.left = false;
}

void Indicator::right() {
	right(!status.right);
}

void Indicator::right(bool enable) {
	if (status.hazard)
		return;
	if (enable) {
		if (status.left)
			status.left = false;
		status.right = true;
	} else
		status.right = false;
}

uint32_t Indicator::getFrontPixel(int i) {
	int timer = millis() % 1000;
	int step = int((timer / 500.) * front_side_pixels) + 1;
	// center
	if (i >= front_side_pixels && i < front_total_pixels - front_side_pixels) {
		if (status.visibility)
			return WHITE;
		else
			return BLACK;
	}
	// left
	if (i < front_side_pixels) {
		if (status.left || status.hazard) {
			if (timer < 500) {
				if (i >= front_side_pixels - step)
					return AMBER;
				else
					return BLACK;
			} else
				return BLACK;
		} else {
			if (status.visibility)
				return WHITE;
			else
				return BLACK;
		}
	}
	// front right
	int offset = front_total_pixels - front_side_pixels;
	if (i >= offset) {
		if (status.right || status.hazard) {
			if (timer < 500) {
				if (i - offset < step)
					return AMBER;
				else
					return BLACK;
			} else
				return BLACK;
		} else {
			if (status.visibility)
				return WHITE;
			else
				return BLACK;
		}
	}
	return BLACK;
}

uint32_t Indicator::getRearPixel(int i) {
	int timer = millis() % 1000;
	int step = int((timer / 500.) * rear_side_pixels) + 1;
	// center
	if (i >= rear_side_pixels && i < rear_total_pixels - rear_side_pixels) {
		if (status.visibility)
			return DARKRED;
		else
			return BLACK;
	}
	// left
	if (i < rear_side_pixels) {
		if (status.left || status.hazard) {
			if (timer < 500) {
				if (i >= rear_side_pixels - step)
					return AMBER;
				else
					return BLACK;
			} else
				return BLACK;
		} else if (status.brake)
			return RED;
		else if (status.visibility)
			return DARKRED;
		else
			return BLACK;
	}
	// right
	int offset = rear_total_pixels - rear_side_pixels;
	if (i >= offset) {
		if (status.right || status.hazard) {
			if (timer < 500) {
				if (i - offset < step)
					return AMBER;
				else
					return BLACK;
			} else
				return BLACK;
		} else {
			if (status.lowbeam || status.highbeam || status.visibility)
				return RED;
			else
				return BLACK;
		}
	}
	return BLACK;
}

void Indicator::loop() {
}
