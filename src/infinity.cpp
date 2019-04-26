#include <ros_test/turtle_abstract.h>

int main(int argc, char **argv)
{	
    ros::init(argc, argv, "ros_test");   
	tutorial::AbstractTurtle turtle;

    // turtle should make infinty sign

	for(int i = 0;i<4;i++)
{
	turtle.arc(1,180);
	turtle.turn(45);
	turtle.forward(sqrt(8));
	turtle.turn(-45);
	turtle.arc(1,-180);
	turtle.turn(-45);
	turtle.forward(sqrt(8));
	turtle.turn(45);


	}

	
    ROS_INFO_STREAM("Finished");

    return 0;
}
