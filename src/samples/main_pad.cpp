#include "ardrone/ardrone.h"

#define KEY_DOWN(key) (GetAsyncKeyState(key) & 0x8000)
#define KEY_PUSH(key) (GetAsyncKeyState(key) & 0x0001)

// --------------------------------------------------------------------------
// main(�����̐��A�������X�g)
// ���C���֐��ł�
// �߂�l ����I��:0 �G���[:-1
// --------------------------------------------------------------------------
int main(int argc, char **argv)
{
    // AR.Drone�N���X
    ARDrone ardrone;

    // ������
    if (!ardrone.open()) {
        printf("ARDrone�̏������Ɏ��s���܂���\n");
        return -1;
    }

    // ���C�����[�v
    while (!GetAsyncKeyState(VK_ESCAPE)) {
        // AR.Drone�̍X�V
        if (!ardrone.update()) break;

        // �摜�̎擾
        IplImage *image = ardrone.getImage();

        //// �i�r�Q�[�V�����f�[�^�̎擾
        //double roll  = ardrone.getRoll();
        //double pitch = ardrone.getPitch();
        //double yaw   = ardrone.getYaw();
        //printf("ardrone.roll  = %3.2f [deg]\n", roll  * RAD_TO_DEG);
        //printf("ardrone.pitch = %3.2f [deg]\n", pitch * RAD_TO_DEG);
        //printf("ardrone.yaw   = %3.2f [deg]\n", yaw   * RAD_TO_DEG);

        // ���x
        double altitude = ardrone.getAltitude();
        printf("ardrone.altitude = %3.2f [m]\n", altitude);

        // ���x
        double vx, vy, vz;
        double velocity = ardrone.getVelocity(&vx, &vy, &vz);
        printf("ardrone.vx = %3.2f [m/s]\n", vx);
        printf("ardrone.vy = %3.2f [m/s]\n", vy);
        printf("ardrone.vz = %3.2f [m/s]\n", vz);

        // �o�b�e���c��
        int battery = ardrone.getBatteryPercentage();
        printf("ardrone.battery = %d [��] (�c���%d��)\n", battery, 12*battery/100);

        // �����E����
        if (KEY_PUSH(VK_SPACE)) {
            if (ardrone.onGround()) ardrone.takeoff();
            else                    ardrone.landing();
        }

        // �ً}��~
        if (KEY_PUSH(VK_RETURN)) ardrone.emergency();

        // AR.Drone����s���
        if (!ardrone.onGround()) {
            // ���x�w��
            double x = 0.0, y = 0.0, z = 0.0, r = 0.0;
            if (KEY_DOWN(VK_UP))    x =  0.5;
            if (KEY_DOWN(VK_DOWN))  x = -0.5;
            if (KEY_DOWN(VK_LEFT))  r =  0.5;
            if (KEY_DOWN(VK_RIGHT)) r = -0.5;
            if (KEY_DOWN('Q'))      z =  0.5;
            if (KEY_DOWN('A'))      z = -0.5;
            //ardrone.move3D(x, y, z, r);

			//�Q�[���p�b�g
			JOYINFOEX JoyInfoEx;

			JoyInfoEx.dwSize = sizeof(JOYINFOEX);
			JoyInfoEx.dwFlags = JOY_RETURNALL; // �S�Ă̏����擾

			if(joyGetPosEx(0, &JoyInfoEx) == JOYERR_NOERROR){ // ����

				int y_pad = -((int)JoyInfoEx.dwXpos - 0x7FFF)/32512.0*100.0;
				int x_pad = -((int)JoyInfoEx.dwYpos - 0x7FFF)/32512.0*100.0;
				int r_pad = -((int)JoyInfoEx.dwZpos - 0x7FFF)/32512.0*100.0;
				int z_pad = ((int)JoyInfoEx.dwRpos - 0x7FFF)/32512.0*100.0;

				printf("X = %d  ",x_pad);
				printf("Y = %d  ",y_pad);
				printf("Z = %d  ",z_pad);
				printf("R = %d\n",r_pad);

				x = 0.5 * x_pad / 100;
				y = 0.5 * y_pad / 100;
				z = 0.5 * z_pad / 100;
				r = 0.5 * r_pad / 100;

				if(JoyInfoEx.dwButtons & JOY_BUTTON1) printf("�{�^��1");
				if(JoyInfoEx.dwButtons & JOY_BUTTON2) if (ardrone.onGround()) ardrone.takeoff();
				if(JoyInfoEx.dwButtons & JOY_BUTTON3) ardrone.landing();
				if(JoyInfoEx.dwButtons & JOY_BUTTON4) printf("�{�^��4");
			}
		ardrone.move3D(x, y, z, r);
		}


        // �J�����؂�ւ�
        static int mode = 0;
        if (KEY_PUSH('C')) {
            // AR.Drone 2.0
            if (ardrone.getVersion() == ARDRONE_VERSION_2) {
                if (mode == 0) mode = 1;
                else           mode = 0;
            }
            // AR.Drone 1.0
            else {
                if (mode == 0) mode = 2;
                else           mode = 0;
            }
            ardrone.setCamera(mode);
        }

        // �\��
        cvShowImage("camera", image);
        cvWaitKey(30);
    }

    // ���悤�Ȃ�
    ardrone.close();

    return 0;
}

