#include "triangle.h"
#include "display.h"

void int_swap(int* a, int* b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}

void fill_flat_bottom_triangle(triangle_t triangle) {
	int x0 = triangle.points[0].x;
	int x1 = triangle.points[1].x;
	int x2 = triangle.points[2].x;
	int y0 = triangle.points[0].y;
	int y1 = triangle.points[1].y;
	int y2 = triangle.points[2].y;

	float inv_slope_1 = (float)(x1 - x0) / (y1 - y0);
	float inv_slope_2 = (float)(x2 - x0) / (y2 - y0);

	float x_start = x0;
	float x_end = x0;

	for (int y = y0; y <= y2; y++) {
		draw_line(x_start, y, x_end, y, triangle.color);
		x_start += inv_slope_1;
		x_end += inv_slope_2;
	}
}

void fill_flat_top_triangle(triangle_t triangle) {
	int x0 = triangle.points[0].x;
	int x1 = triangle.points[1].x;
	int x2 = triangle.points[2].x;
	int y0 = triangle.points[0].y;
	int y1 = triangle.points[1].y;
	int y2 = triangle.points[2].y;

	float inv_slope_1 = (float)(x2 - x0) / (y2 - y0);
	float inv_slope_2 = (float)(x2 - x1) / (y2 - y1);

	float x_start = x2;
	float x_end = x2;

	for (int y = y2; y >= y0; y--) {
		draw_line(x_start, y, x_end, y, triangle.color);
		x_start -= inv_slope_1;
		x_end -= inv_slope_2;
	}
}

void draw_filled_triangle(triangle_t triangle) {
	if (triangle.points[0].y > triangle.points[1].y) {
		int_swap(&triangle.points[0].y, &triangle.points[1].y);
		int_swap(&triangle.points[0].x, &triangle.points[1].x);
	}

	if (triangle.points[1].y > triangle.points[2].y) {
		int_swap(&triangle.points[1].y, &triangle.points[2].y);
		int_swap(&triangle.points[1].x, &triangle.points[2].x);
	}
	
	if (triangle.points[0].y > triangle.points[1].y) {
		int_swap(&triangle.points[0].y, &triangle.points[1].y);
		int_swap(&triangle.points[0].x, &triangle.points[1].x);
	}

	if (triangle.points[0].y == triangle.points[1].y) {
		fill_flat_top_triangle(triangle);
	}
	else if (triangle.points[2].y == triangle.points[1].y) {
		fill_flat_bottom_triangle(triangle);
	}
	else {
		triangle_t flat_bottom_triangle = triangle;
		triangle_t flat_top_triangle = triangle;

		int My = triangle.points[1].y;
		int Mx = ((float)((triangle.points[2].x - triangle.points[0].x) * (triangle.points[1].y - triangle.points[0].y)) / (float)(triangle.points[2].y - triangle.points[0].y)) + triangle.points[0].x;

		flat_bottom_triangle.points[2].x = Mx;
		flat_bottom_triangle.points[2].y = My;

		flat_top_triangle.points[0].x = flat_top_triangle.points[1].x;
		flat_top_triangle.points[0].y = flat_top_triangle.points[1].y;
		flat_top_triangle.points[1].x = Mx;
		flat_top_triangle.points[1].y = My;

		fill_flat_bottom_triangle(flat_bottom_triangle);
		fill_flat_top_triangle(flat_top_triangle);
	}
}