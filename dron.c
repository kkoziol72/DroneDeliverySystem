#include <stdio.h>

#include <time.h>

#include <po_hi_time.h>

#include <string.h>

#include <stdlib.h>

#include <math.h>



float dron_weight = 1.5;

float G = 9.81;



struct net_route {

	float x;

	float y;

	float z;

};



struct GPS {

	float x;

	float y;

};



struct position {

	float x;

	float y;

	float height;

	float direction;

};



struct direction

{

	float direction_horizontal;

	bool landing;

};



struct wind {

	float direction;

	float speed;

};



struct camera {

	float distance1;

	float distance2;

	float distance3;

	float distance4;

	float distance5;

	float distance6;

	float distance7;

	float distance8;

	float distance9;

};



struct control_out {

	float force;

	float angle;

	float direction1;

	float direction2;

	float direction3;

	float direction4;

};





void weight_thread (int weight_in, int weight_net_in, bool* weight_out)

{

    printf ("Weight thread\n");

    struct timespec mytime;

    clock_gettime (CLOCK_REALTIME, &mytime);

    printf ("At time %3lu:%3lu, weight: %d , max_weight: %d ", mytime.tv_sec % 3600, mytime.tv_nsec/1000000,weight_in, weight_net_in);

    if (weight_in <= weight_net_in){

        printf ("Weight is OK\n");

        *weight_out = true;

    }

    else{

        printf ("Weight is not OK\n");

        *weight_out = false;

    }

}



void height_thread(int height_in, int height_net_in, bool* height_out)

{

	printf("Height thread\n");

	struct timespec mytime;

	clock_gettime(CLOCK_REALTIME, &mytime);

	printf("At time %3lu:%3lu, height: %d , max_height: %d ", mytime.tv_sec % 3600, mytime.tv_nsec / 1000000, height_in, height_net_in);

	if (height_in >= height_net_in) {

		printf("Height is OK\n");

		*height_out = true;

	}

	else {

		printf("Height is not OK\n");

		*height_out = false;

	}

}



void network_module_communication_in_thread (char* network_in, float* net_weight_out, struct net_route* net_route_out)

{

	int size = sizeof(network_in) / sizeof(network_in[0]);



	int counter = 1;

	char* temp = strtok(network_in, ',');

	while (temp != nanl)

	{

		if (counter == 1)

		{

			*net_weight_out = atof(temp);

		}

		else if (counter == 2)

		{

			(*net_route_out).x = atof(temp);

		}

		else if (counter == 3)

		{

			(*net_route_out).y = atof(temp);

		}

		else if (counter == 4)

		{

			(*net_route_out).z = atof(temp);

		}

		counter++;

		temp = strtok(nanl, ',');

	}

}





void route_thread(struct net_route net_in, struct GPS GPS_in, float compass_in, float height_in, struct direction* direc_out, struct position* pos_out)

{

    printf ("Route thread\n");



	(*direc_out).direction_horizontal = atan2(net_in.y - GPS_in.y, net_in.x - GPS_in.x) - compass_in;

	(*direc_out).landing = (net_in.x == GPS_in.x && net_in.y == GPS_in.y);

	

    (*pos_out).x = GPS_in.x;

    (*pos_out).y = GPS_in.y;

    (*pos_out).height = height_in;



    struct timespec mytime;

    clock_gettime (CLOCK_REALTIME, &mytime);

    printf ("At time %3lu:%3lu, landing: %d, direction_horizontal: %f, pos_x: %f, pos_y: %f, pos_height: %f ", mytime.tv_sec % 3600, mytime.tv_nsec/1000000,(*direc_out).landing, (*direc_out).direction_horizontal, (*pos_out).x, (*pos_out).y, (*pos_out).height);

}





void fly_thread (bool weight_is_ok,/* float weight, bool height,*/ struct direction route_in, struct wind wind, struct camera camera_in, struct control_out* control_out)

{



float weight = 20.0;

bool height = 20;

	printf("Fly thread\n");

	if (weight_is_ok)

	{

		if (route_in.landing)

		{

			printf("Fly thread -> landing\n");

		}

		else 

		{

			if (height)

			{

				if (camera_in.distance5 > 10.0)

				{

					if (route_in.direction_horizontal == 0)

					{

						set_control(weight, wind, control_out);

					}

					else {

						set_control_rotation(weight, wind, route_in, control_out);

					}

				}



			}

			else

			{



			}

		}

	}

	else

	{



	}



}



void set_control(float weight, struct wind wind, struct control_out* control_out) {



	(*control_out).angle = acos((weight + dron_weight) * G / (*control_out).force);

	if (wind.direction < M_PI) {

		float dir = asin((sin(wind.direction) * wind.speed) / ((*control_out).force * sin((*control_out).angle)));

		(*control_out).direction1 = dir;

		(*control_out).direction2 = dir;

		(*control_out).direction3 = dir;

		(*control_out).direction4 = dir;

	}



}



void set_control_rotation(float weight, struct wind wind, struct direction direction, struct control_out* control_out) {



	(*control_out).angle = acos((weight + dron_weight) * G / (*control_out).force);

	if (wind.direction < M_PI) {

		float dir = asin((sin(wind.direction) * wind.speed) / ((*control_out).force * sin((*control_out).angle)));

		(*control_out).direction1 = dir;

		(*control_out).direction2 = dir;

		(*control_out).direction3 = dir;

		(*control_out).direction4 = dir;

	}



}





void network_module_communication_out_thread (int weight_in, struct position pos_in, char* net_out)

{

    struct timespec mytime;

    clock_gettime (CLOCK_REALTIME, &mytime);

    printf ("Network module out thread\n");

    printf ("At time %3lu:%3lu, weight: %d , pos_x: %f , pos_y: %f , pos_height: %f\n", mytime.tv_sec % 3600, mytime.tv_nsec/1000000,weight_in, pos_in.x, pos_in.y, pos_in.height);



    char* tmp1 = itoa (weight_in,net_out,10);

    int size1 = sizeof(tmp1)/sizeof(tmp1[0]);

    char* tmp2 = ftoa (pos_in.x,net_out,10);

    int size2 = sizeof(tmp2)/sizeof(tmp2[0]);

    char* tmp3 = ftoa (pos_in.y,net_out,10);

    int size3 = sizeof(tmp3)/sizeof(tmp3[0]);

    char* tmp4 = ftoa (pos_in.height,net_out,10);

    int size4 = sizeof(tmp4)/sizeof(tmp4[0]);



    for(int i = 0; i < size1; i++){

        net_out[i] = tmp1[i];

    }

    net_out[size1] = ',';

    for(int i = 0; i < size2; i++){

        net_out[i] = tmp2[i];

    }

    net_out[size2] = ',';

    for(int i = 0; i < size3; i++){

        net_out[i] = tmp3[i];

    }

    net_out[size3] = ',';

    for(int i = 0; i < size4; i++){

        net_out[i] = tmp4[i];

    }



}

