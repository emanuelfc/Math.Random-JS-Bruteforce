#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int32_t rng[2]; // Final rng state

// From https://github.com/nodejs/node-v0.x-archive/blob/61c6abf00898fe00eb7fcf2c23ba0b01cf12034c/deps/v8/src/math.js#L146
double MathRandom()
{
	int32_t r0 = ((18273 * (rng[0] & 0xFFFF)) + (rng[0] >> 16)) | 0;
	rng[0] = r0; // rng[0] = 3-2 bytes + 0-1 bytes de rng[0] | 0
	int32_t r1 = ((36969 * (rng[1] & 0xFFFF)) + (rng[1] >> 16)) | 0;
	rng[1] = r1; // rng[1] = 3-2 bytes + 0-1 bytes de rng[1] | 0

	int32_t x = ((r0 << 16) + (r1 & 0xFFFF)) | 0; // random = 1-2 bytes rng[0] + 1-2 bytes rng[1]
	// preserva low bytes de r1 e low bytes de r0

	return (x < 0 ? (x + 0x100000000) : x) * 2.3283064365386962890625e-10;
}

// Bruteforce method
void bruteforce(double first, double second)
{
	// From previous multiplication

	uint32_t x1 = first / 2.3283064365386962890625e-10;     // multiplicamos antes
	uint32_t x2 = second / 2.3283064365386962890625e-10;    // multiplicamos antes

    uint32_t rng0_up = x1 >> 16;         // 3-2 bytes de x1 - 1-2 bytes de rng0
    uint32_t rng0_low = x1 & 0xFFFF;     // 1-2 bytes de x1 - 1-2 bytes de rng1

    uint32_t rng1_up = x2 >> 16;         // 3-2 bytes de x2 - 1-2 bytes de rng0
    uint32_t rng1_low = x2 & 0xFFFF;     // 1-2 bytes de x2 - 1-2 bytes de rng1

    int found = 0;
    int cur = 0;

	uint32_t r0 = 0; // r0 = rng0

	// Solve rng0
	printf("Solving 3rd and 2nd rng state bytes\n\n");
	while(cur <= 0xFFFF && !found)
	{
		// x = ((r0 << 16) + (r1 & 0xFFFF)) | 0;
		r0 = ((cur << 16) + rng0_up) | 0;
        // r0 = ((18273 * (rng[0] & 0xFFFF)) + (rng[0] >> 16)) | 0;
		r0 = ((18273 * (r0 & 0xFFFF)) + (r0 >> 16)) | 0;

		if((r0 & 0x0000FFFF) == rng1_up)
		{
		    printf("%d\n", r0);
		    found = 1;
		}
		else cur++;
	}

    if(found == 0) return;
    else printf("Found 3rd and 2nd rng state bytes: %d\n", r0);
    found = 0;

    uint32_t r1 = 0; // r1 = rng1

    printf("Solving 1rd and 0nd rng state bytes\n\n");
	while(cur <= 0x0000FFFF && !found)
	{
        // x = ((r0 << 16) + (r1 & 0xFFFF)) | 0;
        r1 = ((cur << 16) + rng0_low) | 0;
        // r1 = ((36969 * (rng[1] & 0xFFFF)) + (rng[1] >> 16)) | 0;
		r1 = ((36969 * (r1 & 0xFFFF)) + (r1 >> 16)) | 0;

		if((r1 & 0x0000FFFF) == rng1_low) found = 1;
		else cur++;
	}

    if(found == 0) return;
    else printf("Found 1rd and 0nd rng state bytes: %d\n", r1);

    printf("Done!\n");

    rng[0] = r0;
    rng[1] = r1;

	printf("Next Double: %f \n", MathRandom());
}

int main(int argc, char **argv)
{
  if(argc < 2) return -1;

  printf("first: %f\n", atof(argv[1]));
  printf("second: %f\n", atof(argv[2]));

  bruteforce(atof(argv[1]), atof(argv[2]));
  //bruteforce(0.20136196492239833,0.6620366568677127); =  237939067 e 1109015550 e next = 0.5118521023541689 com 18030 no MathRandom

  return 0;
}
