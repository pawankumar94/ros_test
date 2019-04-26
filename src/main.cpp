

#include <ros_test/turtle_abstract.h>

int main(int argc, char **argv)
{	
    ros::init(argc, argv, "ros_test");   
	tutorial::AbstractTurtle turtle;

    // turtle should run in a square with length 3
	
	   
for (int i = 0; i < 4; i++)
{
        turtle.forward(3);
        turtle.turn(90);
   }

    ROS_INFO_STREAM("Finished");

    return 0;
}
