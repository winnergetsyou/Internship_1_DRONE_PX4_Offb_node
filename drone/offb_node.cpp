
#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>

#include <math.h>
// included the math.h library for using mathematical operations and functions like sine.

#include <ros/duration.h>
#include <time.h>

mavros_msgs::State current_state;
void state_cb(const mavros_msgs::State::ConstPtr& msg){
    current_state = *msg;
}
int main(int argc, char **argv)
{
    ros::init(argc, argv, "offb_node");
    ros::NodeHandle nh;

    ros::Subscriber state_sub = nh.subscribe<mavros_msgs::State>
            ("mavros/state", 10, state_cb);
    ros::Publisher local_pos_pub = nh.advertise<geometry_msgs::PoseStamped>
            ("mavros/setpoint_position/local", 10);
    ros::ServiceClient arming_client = nh.serviceClient<mavros_msgs::CommandBool>
            ("mavros/cmd/arming");
    ros::ServiceClient set_mode_client = nh.serviceClient<mavros_msgs::SetMode>
            ("mavros/set_mode");

    //the setpoint publishing rate MUST be faster than 2Hz
    ros::Rate rate(20.0);

    int l = 1 ;
    float jj = 1 ;
    float jz = 0 ;

for (int tm = 0 ; tm < 30 ; tm++)
{

	ROS_INFO("looping");
  
// creates a sinusoidal wave pattern
  
    geometry_msgs::PoseStamped pose;
    for (int nm = 1 ;nm < 1000050 ; nm++)
    { jj = (1/nm) * sin (nm * 3.14 * l / 8 );
	    jz = jz + jj ;
    }
  
  
    pose.pose.position.x = l; // actually the distance in earth frame 
    pose.pose.position.y =  0; // actually the distance in earth frame X axis
    pose.pose.position.z = (1.27388535 * jz) + 2 ; // The height of UAV
    pose.pose.orientation.x = 0;
    pose.pose.orientation.y = 0;
    pose.pose.orientation.z = 0;
    pose.pose.orientation.w = 0;


for(int i = 10; ros::ok() && i > 0; --i){
        local_pos_pub.publish(pose);
        ros::spinOnce();
        rate.sleep();
    }



    mavros_msgs::SetMode offb_set_mode;
    offb_set_mode.request.custom_mode = "OFFBOARD";

    mavros_msgs::CommandBool arm_cmd;
    arm_cmd.request.value = true;

    ros::Time last_request = ros::Time::now();
int xx = 1 ;

while(ros::ok() && xx == 1){

        if( current_state.mode != "OFFBOARD" &&
            (ros::Time::now() - last_request > ros::Duration(2.0))){
            if( set_mode_client.call(offb_set_mode) &&
                offb_set_mode.response.mode_sent){
                ROS_INFO("Offboard enabled");
            }
            last_request = ros::Time::now();
        } else {
            if( !current_state.armed &&
                (ros::Time::now() - last_request > ros::Duration(2.0))){
                if( arming_client.call(arm_cmd) &&
                    arm_cmd.response.success){
                    ROS_INFO("Vehicle armed");
                }
                last_request = ros::Time::now();
		ROS_INFO("ajnds1");
            }
        }

	if (ros::Time::now() - last_request < ros::Duration(5))
	{
	 xx = 1;
	}else {
	xx = 0 ;
	}

	ROS_INFO("ajnds2");        
	local_pos_pub.publish(pose);

	ROS_INFO("ajnds3");
        ros::spinOnce();
 
ROS_INFO("ajnds4");
rate.sleep();

    
}
l++;
 ros::spinOnce();
ROS_INFO("ajnds");
ros::Duration(1).sleep();
ROS_INFO("ajnds");
}

    return 0;
}

