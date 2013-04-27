#include <Math.h>
static int* average(int* vectors, int width, int edgelength){
	int len;
	static int av[2];
	int sumx, sumy;
	for(len=0;len<edgelength;len++){
		sumx+=vectors[len]%width;
		sumy+=vectors[len]/width;
		//len++;
	}
	av[0]=sumx/edgelength;
	av[1]=sumy/edgelength;
	return av;
}
static int* maxdistances(int numpts, int* vectors, int* avg,int width, int edgelength){
	int* pts=(int*)malloc(sizeof(int)*numpts);
	int len;
	int i;
	double dist=0;
	for(len=0;len<edgelength;len++){
		dist=(vectors[len]%width-avg[0])*(vectors[len]%width-avg[0])
			+(vectors[len]/width-avg[1])*(vectors[len]/width-avg[1]);
		for(i=0;i<numpts;i++){
			if(dist>pts[i])
			{
				pts[i]=dist;
				break;
			}
		}
		//len++;
	}
	return pts;
}
static int* reorder(int numpts, int* maxima, int* avg, int width){
	double *thetas=(double*)malloc(sizeof(double)*numpts);
	int* nmax=(int*)malloc(sizeof(int)*numpts);
	//int minustheta=0;
	int minind, mintheta;
	int i=0,n=0;
	for(i=0;i<numpts;i++){
		thetas[i]=atan2((avg[1]-maxima[i]/width),(avg[0]-maxima[i]%width));
	}
	for(n=0;n<numpts;n++){
		mintheta=thetas[0];
		minind=0;
		for(i=0;i<numpts;i++){
			if(mintheta>thetas[i]){
				mintheta=thetas[i];
				minind=i;
			}

		}
		nmax[n]=maxima[minind];

		thetas[minind]=10;
		//minustheta+=thetas[minind];
		//maxima[maxind]={0,0};
	}
	free(thetas);
	return nmax;
}
static int* getCHull(int* edge, int width, int edgelength){
	int* center=average(edge, width, edgelength);
	int* dists=maxdistances(8,edge,center, width, edgelength);
	free(dists);
	return reorder(8,dists,center,width);
}