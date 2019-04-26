#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <turtlesim/Pose.h>

namespace tutorial {

class AbstractTurtle {

    ros::Publisher pub_cmd_vel;
    ros::Subscriber sub_pose;
    bool ready;
    turtlesim::Pose current_pose;

    geometry_msgs::Twist go_forward;
    geometry_msgs::Twist turn_left;
    geometry_msgs::Twist turn_right;
    geometry_msgs::Twist arc_velo_ccw;
	geometry_msgs::Twist arc_velo_cw;
  
    geometry_msgs::Twist stop;


    void poseCallback( const turtlesim::Pose::ConstPtr& msg_in ) {
        this->ready = true;
        this->current_pose = *msg_in;
        this->current_pose.theta = fmod( this->current_pose.theta + 2*M_PI, 2*M_PI );
   
	}

public:
    AbstractTurtle( double velocity = 1.0 ) : ready(false) {

        ros::NodeHandle pnh("~");
        double ros_param_velocity;
        pnh.param<double>("velocity", ros_param_velocity, -1.0);
        if ( ros_param_velocity > 0 ) { velocity = ros_param_velocity; }

        this->go_forward.linear.x = velocity;
        this->stop.linear.x = 0;
        this->stop.angular.z = 0;
        this->turn_left.angular.z = velocity;
        this->turn_right.angular.z = (-1) * velocity;
        this->arc_velo_ccw.linear.x = velocity;
	this->arc_velo_cw.linear.x = velocity;

        ros::NodeHandle nh;

        this->pub_cmd_vel = nh.advertise<geometry_msgs::Twist>("turtle1/cmd_vel", 1);
        this->sub_pose = nh.subscribe<turtlesim::Pose>("/turtle1/pose", 1, &AbstractTurtle::poseCallback, this);

        while ( !this->ready && ros::ok() ) {
            ros::Duration(0.2).sleep();
            ros::spinOnce();
        }
        ROS_INFO( "Turtle is ready. velocity = %.3f", velocity );
    }

    /**
        param length [meter]
    **/

    void forward ( double length ) {
        if ( length <= 0 ) { return; }

        double start_x = this->current_pose.x;
        double start_y = this->current_pose.y;
        double d_sqr = 0;
        ROS_DEBUG("[FORWARD] length = %.3lf", length);

        do {
            this->pub_cmd_vel.publish( this->go_forward );
            if ( !ros::ok() ) { return; }
            ros::Duration(0.001).sleep();
            ros::spinOnce();
            d_sqr = (this->current_pose.x - start_x) * (this->current_pose.x - start_x) + (this->current_pose.y - start_y) * (this->current_pose.y - start_y);
        } while ( d_sqr < length * length );

        this->pub_cmd_vel.publish( this->stop );
	
    }

    /**
        param angle [degree]
    **/

    void turn ( double angle ) {
        if ( fabs(angle) < 1 ) { return; }

        double target_angle = this->current_pose.theta + angle / 180 * M_PI;
        target_angle = fmod( target_angle + 2*M_PI, 2*M_PI );
        ROS_DEBUG("[TURN] target_angle = %.3f", target_angle );

        do {
            if ( angle > 0 ) {
                this->pub_cmd_vel.publish( this->turn_left );
            } else {
                this->pub_cmd_vel.publish( this->turn_right );
            }
            if ( !ros::ok() ) { return; }
            ros::Duration(0.001).sleep();
            ros::spinOnce();
        } while ( fabs(target_angle - this->current_pose.theta) > 0.01 );

        this->pub_cmd_vel.publish( this->stop );
    }

    void arc_length ( double radius, double length ) {
        if ( fabs(radius) < 0.01 ) { return; }
        arc( radius, fabs(length / radius) * 180 / M_PI );
    }

    /**
        param radius [meter]
        param angle [degree]
    **/
    void arc(double radius, float angle){
      double target_theta = this->current_pose.theta + (angle / 180) * M_PI;

      ROS_INFO( "target theta = %f Current thetha = %f radius = %f", target_theta, this->current_pose.theta, radius );

      do {

          if ( angle > 0 ) {
              this->arc_velo_ccw.angular.z = (this->arc_velo_ccw.linear.x) / (radius);
              ROS_INFO( "Angular: %f target = %f", this->arc_velo_ccw.angular.z, this->current_pose.theta );
              this->pub_cmd_vel.publish( this->arc_velo_ccw );
          } else {

              this->arc_velo_cw.angular.z = (-1) * (this->arc_velo_cw.linear.x) / (radius);
              ROS_INFO( "CAngular: %f target = %f", this->arc_velo_cw.angular.z, this->current_pose.theta );
              this->pub_cmd_vel.publish( this->arc_velo_cw );

          }

          if ( !ros::ok() ) { return; }
          ros::Duration(0.001).sleep();
          ros::spinOnce();

      } while ( fabs(target_theta - this->current_pose.theta) > 0.01 );
      this->pub_cmd_vel.publish( this->stop );

    }

}; // class AbstractTurtle

} // ns tutorial
