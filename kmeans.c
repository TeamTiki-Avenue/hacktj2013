#include <stdlib.h>
//#include <stdio.h> // only for testing purposes
//#include <time.h> // only for testing purposes
//#include <math.h> // only for testing purposes
/*
 * This is the point that moves around the screen in order to have an OOP-ish implementation of the K-Means Clustering algorithm
 * Remember to set each value to something before you use it,
 * default values should be {kk/width, kk/height, kk/k, NULL, 0, 0} where kk is an iterating integer, width is the width of the image,
 * height is the height of the image, k is the total number of kPoints.
 *
 * @attribute x
 * 		Represents the average x position of the points it owns
 * @attribute y
 * 		Represents the average y position of the points it owns
 * @attribute hue
 * 		Represents the average hue of the points it owns
 * @attribute owned
 * 		A dynamic array that contains the indices of all of the points it owns;
 * 		each index should be equal to y*width+x with the variables x and y representing the position of the pixel and width representing the width of the image
 * @attribute numOwned
 *		Number of points owned by the kPoint
 * @attribute numAccumulated
 * 		Size of owned
 */
typedef struct {
	double x;
	double y;
	double hue;
	int * owned;
	int numOwned, numAccumulated;
} kPoint;

typedef struct {
	int * owned;
	int numOwned, numAccumulated;
} queue;

int addToQueue(queue * q, int index, int minimumSize) {
	if (q->numOwned == q->numAccumulated) {
		if (q->numAccumulated == 0) {
			q->numAccumulated = minimumSize;
		} else {
			q->numAccumulated += minimumSize;
		}

		void * temp = realloc(q->owned, q->numAccumulated * sizeof(int));

		if (temp == NULL ) {
			//printf("Got a null pointer\n");
			free(temp);
			return 1;
		} else {
			////printf("%d %d\n", kp->owned, temp);
			q->owned = (int *) temp;
			////printf("%d %d\n", kp->owned, temp);
		}
	}

	q->owned[q->numOwned] = index;
	q->numOwned++;

	////printf("sizes %d %d\n", kp->numOwned, kp->numAccumulated);

	return 0;
}

/*
 * Adds a pixel's index to the list of pixels that a kPoint owns.  The list is a dynamic array, so it will be resized if necessary.
 *
 * @arg kp
 * 		It is the (reference to) a kPoint
 * @arg index
 * 		Index that you want to add to the ownership of kp
 * @arg minimumSize
 * 		Minimum size that you want the array that holds all of the indexes to be,
 * 		making this equal to the total number of pixels divided by the number of kPoints would probably work best
 *
 * @result
 * 		The value returned will state success (0) or failure (1+)
 */
int addToOwned(kPoint * kp, int index, int minimumSize) {
	if (kp->numOwned == kp->numAccumulated) {
		if (kp->numAccumulated == 0) {
			kp->numAccumulated = minimumSize;
		} else {
			kp->numAccumulated += minimumSize;
		}

		void * temp = realloc(kp->owned, kp->numAccumulated * sizeof(int));

		if (temp == NULL ) {
			//printf("Got a null pointer\n");
			free(temp);
			return 1;
		} else {
			////printf("%d %d\n", kp->owned, temp);
			kp->owned = (int *) temp;
			////printf("%d %d\n", kp->owned, temp);
		}
	}

	kp->owned[kp->numOwned] = index;
	kp->numOwned++;

	////printf("sizes %d %d\n", kp->numOwned, kp->numAccumulated);

	return 0;
}

/*
 * Moves the kPoint to its new location, based off of the points it owns
 *
 * @arg kp
 * 		It is the (reference to) a kPoint
 * @arg hues
 * 		It is the (reference to) an array of hue values per pixel
 * @arg width
 * 		Width of the image
 * @result
 * 		Returns the squared distance between where the kPoint was and now is
 */
double adjustPosition(kPoint * kp, double * hues, int width) {
	int x = 0, y = 0;
	double hue = 0;

	for (int i = 0; i < kp->numOwned; i++) {
		x += kp->owned[i] % width;
		y += kp->owned[i] / width;
		hue += hues[kp->owned[i]];
	}

	double oldHue = kp->hue;

	kp->x = x / (double) kp->numOwned;
	kp->y = y / (double) kp->numOwned;
	kp->hue = hue / kp->numOwned;

	return (oldHue - kp->hue) * (oldHue - kp->hue);
}

/*
 * Does the actual k-means clustering technique, reapportioning the points to all of the kPoints
 *
 * @arg kps
 * 		It is an array of kPoints (should be in reference to a reference format)
 * @arg k
 * 		Number of kPoints
 * @arg hues
 * 		It is the (reference to) an array of hue values per pixel
 * @arg size
 * 		It is the total number of pixels in the picture
 */
void distributePoints(kPoint * kps, int k, double * hues, int size) {
	//printf("entered distribute points\n");
	//printf("first kpoint exists %d\n", kps[0].numOwned);
	for (int j = 0; j < k; j++) {
		//printf("we're going through %d kpoint (0x%p)\n", j, &kps[j]);
		kps[j].numOwned = 0;
	}

	//printf("set stuff to 0\n");
	//for (int i = 0; i < k; i++) {
	//printf("i %d x %f y %f hue %f\n", i, kps[i].x, kps[i].y, kps[i].hue);
	//}

	for (int i = 0; i < size; i++) {
		int min = 0;
		for (int j = 0; j < k; j++) {
			//printf("i %d, j %d, min %d, kps[j].hue %f, kps[min].hue %f, hues[i] %f\n", i, j, min, kps[j].hue, kps[min].hue, hues[i]);
			if ((kps[j].hue - hues[i]) * (kps[j].hue - hues[i]) < (kps[min].hue - hues[i]) * (kps[min].hue - hues[i])) {
				min = j;
				//printf("min was reassigned to %d\n", j);
			}
		}
		addToOwned(&kps[min], i, size / k);
	}

	//for (int i = 0; i < k; i++) {
	////printf("i = %d, numowned = %d, numallo = %d\n", i, kps[i].numOwned, kps[i].numAccumulated);
	//}
	//printf("exit distribution\n");
}

int binarySearchForKPoint(kPoint * kps, int k, int searchedFor) {
	for (int i = 0; i < k; i++) {
		int min = 0, max = kps[i].numOwned - 1;
		while (max >= min) {
			int mid = (max + min) / 2;
			if (kps[i].owned[mid] < searchedFor) {
				min = mid + 1;
			} else if (kps[i].owned[mid] > searchedFor) {
				max = mid - 1;
			} else if (kps[i].owned[mid] == searchedFor) {
				return i;
			}
		}
	}
	return -1;
}

int binarySearchInKPoint(kPoint kp, int searchedFor) {
	int min = 0, max = kp.numOwned - 1;
	while (max >= min) {
		int mid = (max + min) / 2;
		if (kp.owned[mid] < searchedFor) {
			min = mid + 1;
		} else if (kp.owned[mid] > searchedFor) {
			max = mid - 1;
		} else if (kp.owned[mid] == searchedFor) {
			return mid;
		}
	}
	return -1;
}

int inQueue(queue q, int i) {
	for (int x = 0; x < q.numOwned; x++) {
		if (q.owned[x] == i) {
			return 1;
		}
	}
	return 0;
}

queue getEdge(kPoint * kps, int k, int width, int height) {
	int middle = width / 2 + height / 2;
	int kk = binarySearchForKPoint(kps, k, middle);

	queue q = { NULL, 0, 0 };
	addToQueue(&q, middle, width * height / k);

	int index = middle;
	int directionTo = 0;
	do {
		int temp = 0;
		if (directionTo == 0) {
			temp = -(width + 1);
		} else if (directionTo == 1) {
			temp = -width;
		} else if (directionTo == 2) {
			temp = -(width - 1);
		} else if (directionTo == 3) {
			temp = 1;
		} else if (directionTo == 4) {
			temp = width + 1;
		} else if (directionTo == 5) {
			temp = width;
		} else if (directionTo == 6) {
			temp = width - 1;
		} else {
			temp = -1;
		}
		int binary = binarySearchInKPoint(kps[kk], index + temp);
		if (binary >= 0) {
			if (inQueue(q, index + temp)) {
				break;
			}
			addToQueue(&q, index + temp, width * height / k);
			index += temp;
			directionTo -= 1;
		} else {
			directionTo += 1;
		}

		directionTo = directionTo < 0 ? 7 : directionTo > 7 ? 0 : directionTo;

	} while (1);

	void * temp = realloc(q.owned, q.numOwned * sizeof(int));

	if (temp == NULL ) {
		//printf("Got a null pointer\n");
		free(temp);
	} else {
		////printf("%d %d\n", kp->owned, temp);
		q.owned = (int *) temp;
		////printf("%d %d\n", kp->owned, temp);
	}

	return q;
}

queue main2(int width, int height, double * hues, int k) {
	//printf("Entered main\n");

	//int width = 720;
	//int height = 480;

	//srand(time(NULL ));

	//printf("width*height = %d\n", width * height * sizeof(double));
	//double * hues = malloc(width * height * sizeof(double));
//printf("capable of making the damn array\n");
	/*
	 for (int y = 0; y < height; y++) {
	 for (int x = 0; x < width; x++) {
	 hues[y * width + x] = rand() / (double) RAND_MAX * 0.1;
	 if (sqrt((y - height / 2.0) * (y - height / 2.0) + (x - width / 2.0) * (x - width / 2.0)) < 50
	 || sqrt((y - height / 3.0) * (y - height / 3.0) + (x - width / 3.0) * (x - width / 3.0)) < 20) {
	 hues[y * width + x] += 0.8;
	 }
	 }
	 }*/

	/*
	 FILE * huepic = fopen("huepic.ppm", "w");
	 fprintf(huepic, "P3\n%d %d\n255\n", width, height);
	 for (int y = 0; y < height; y++) {
	 for (int x = 0; x < width; x++) {
	 fprintf(huepic, "%d %d %d \n", (int) (hues[y * width + x] * 255), (int) (hues[y * width + x] * 255), (int) (hues[y * width + x] * 255));
	 }
	 }
	 fflush(huepic);
	 fclose(huepic);
	 */

//printf("Made hue pic\n");
	//int k = 2;
	kPoint * kps = (kPoint *)malloc(k * sizeof(kPoint));
	for (int i = 0; i < k; i++) {
		kps[i].x = (width * i) / ((double) k);
		kps[i].y = (height * i) / ((double) k);
		kps[i].hue = 0.25 + (i) / ((double) k);
		//printf("i %d x %f y %f hue %f\n", i, kps[i].x, kps[i].y, kps[i].hue);
		kps[i].owned = NULL;
		kps[i].numOwned = 0;
		kps[i].numAccumulated = 0;
	}

	//printf("Made the kPoints\n");

	//clock_t beginKmeans = clock();

	double totalAdjustment;
	do {
		//printf("going through iteration\n");
		distributePoints(kps, k, hues, width * height);
		//printf("distributed points\n");
		//for (int i = 0; i < k; i++) {
		//printf("i = %d, numowned = %d\n", i, kps[i].numOwned);
		//}
		totalAdjustment = 0;
		for (int i = 0; i < k; i++) {
			totalAdjustment += adjustPosition(&kps[i], hues, width);
		}

	} while (totalAdjustment > 0.1);

	/*
	 for (int i = 0; i < k; i++) {
	 //printf("i = %d, numowned = %d\n", i, kps[i].numOwned);
	 }
	 */

	/*
	 FILE * kmeanspic = fopen("kmeanspic.ppm", "w");
	 fprintf(kmeanspic, "P3\n%d %d\n255\n", width, height);
	 */

	/*
	 int * currentPosition = malloc(k * sizeof(int));
	 for (int i = 0; i < k; i++) {
	 currentPosition[i] = 0;
	 }
	 int index = 0;
	 while (index + 1 < width * height) {
	 for (int i = 0; i < k; i++) {
	 while (currentPosition[i] < kps[i].numOwned && kps[i].owned[currentPosition[i]] == index) {
	 //printf("i %d val %d\n", i, (int) (255 * i / ((double) k)));
	 //fprintf(kmeanspic, "%d %d %d \n", (int) (255 * i / ((double) k)), (int) (255 * i / ((double) k)), (int) (255 * i / ((double) k)));
	 currentPosition[i]++;
	 index++;
	 }
	 }
	 }

	 clock_t endKmeans = clock();
	 printf("time taken was %f\n", (endKmeans - beginKmeans) / (double) CLOCKS_PER_SEC );
	 */

	/*
	 fflush(kmeanspic);
	 fclose(kmeanspic);
	 */

	//free(hues);
	//free(currentPosition);
	queue edge = getEdge(kps, k, width, height);

	for (int i = 0; i < k; i++) {
		free(kps[i].owned);
	}
	free(kps);

	return edge;
}

