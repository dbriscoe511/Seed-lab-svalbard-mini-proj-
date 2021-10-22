open_system = ("camera_1");

Va_1 = 0.5;
Va_2 = 0.5;
Theta1_dot = 0.5; %measured velocity of wheel 1
Theta2_dot = 0.5; %measured velocity of wheel 2
r = 3;
d = 11.125;
P_dot = (r*(Theta1_dot + Theta2_dot))/2;
Phi_dot = (r*(Theta1_dot - Theta2_dot))/d;
P_desired = 0;
Phi_desired = 0;
