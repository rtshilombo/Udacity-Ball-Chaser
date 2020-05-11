#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>


// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    if (!client.call(srv))
        ROS_ERROR("Failed to call service ball_chaser/command_robot");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    bool white_image = false;
    int img_size= img.height*img.step;
    int split = img.step/3;  //divide image in 3
    int flag; 
    int counter = 0;

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    for (int i=0; i<img_size;i++)
	{
		if (counter<=split)
			flag = 1;
		else if (counter > split && counter <= 2*split)
			flag = 2;
		else 
			flag = 3;
		if (img.data[i] == white_pixel){
			white_image = true;
			break;
			}	
	   counter++;
	   if (counter >= 3*split)
		counter = 0;
	}


    // Then, identify if this pixel falls in the left, mid, or right side of the image
	if(white_image == true)
		{  
			if (flag ==1)
 			   {
				drive_robot(0.5,0.3);
			   }  //drive to the left

			else if(flag ==2)
		 	   {
			drive_robot(0.5,0.0);
		 	   }  //drive foward

			else drive_robot(0.5,-0.3); //drive to the right
		}
	
	// Request a stop when there's no white ball seen by the camera
	else drive_robot(0.0,0.0);
    
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
