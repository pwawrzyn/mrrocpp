/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 2.01  */

#include "../irp6_m/wgt_irp6_m_motors.h"
#include "../irp6_m/wgt_irp6_m_joints.h"

#include "ui_r_irp6_m.h"
#include "../base/ui_ecp_robot/ui_ecp_r_common_012.h"

#include "../base/interface.h"

#include "../base/mainwindow.h"
#include "ui_mainwindow.h"
#include "../base/menu_bar.h"
#include "../base/menu_bar_action.h"
#include "../base/mp.h"

namespace mrrocpp {
namespace ui {
namespace irp6_m {

const std::string UiRobot::WGT_JOINTS = "WGT_JOINTS";
const std::string UiRobot::WGT_MOTORS = "WGT_MOTORS";
const std::string UiRobot::WGT_ANGLE_AXIS = "WGT_ANGLE_AXIS";
const std::string UiRobot::WGT_EULER = "WGT_EULER";
const std::string UiRobot::WGT_RELATIVE_ANGLE_AXIS = "WGT_RELATIVE_ANGLE_AXIS";
const std::string UiRobot::WGT_TOOL_ANGLE_AXIS = "WGT_TOOL_ANGLE_AXIS";
const std::string UiRobot::WGT_TOOL_EULER = "WGT_TOOL_EULER";
//
//
// KLASA UiRobot
//
//
void UiRobot::synchronise()
{
}

int UiRobot::execute_motor_motion()
{
	try {

		ui_ecp_robot->move_motors(desired_pos);

	} // end try
	CATCH_SECTION_IN_ROBOT

	return 1;
}

int UiRobot::execute_joint_motion()
{
	try {

		ui_ecp_robot->move_joints(desired_pos);

	} // end try
	CATCH_SECTION_IN_ROBOT

	return 1;
}

int UiRobot::synchronise_int()

{

	interface.set_ui_state_notification(UI_N_SYNCHRONISATION);

	// wychwytania ew. bledow ECP::robot
	try {
		// dla robota irp6_on_track

		if ((is_edp_loaded()) && (state.edp.is_synchronised == false)) {
			ui_ecp_robot->ecp->synchronise();
			state.edp.is_synchronised = ui_ecp_robot->ecp->is_synchronised();
		} else {
			// 	printf("edp irp6_on_track niepowolane, synchronizacja niedozwolona\n");
		}

	} // end try
	CATCH_SECTION_IN_ROBOT

	// modyfikacje menu
	interface.manage_interface();
	wgts[WGT_MOTORS]->synchro_depended_init();
	wgts[WGT_MOTORS]->init_and_copy();

	return 1;

}

UiRobot::UiRobot(common::Interface& _interface, lib::robot_name_t _robot_name, int _number_of_servos) :
		common_012::UiRobot(_interface, _robot_name, _number_of_servos)
{
}

void UiRobot::manage_interface()
{

	common_012::UiRobot::manage_interface();

	switch (state.edp.state)
	{
		case common::UI_EDP_INACTIVE:

			break;
		case common::UI_EDP_OFF:

			menu_Absolute_Moves->setEnabled(false);
			menu_Relative_Moves->setEnabled(false);
			menu_Tool->setEnabled(false);

			break;
		case common::UI_EDP_WAITING_TO_START_READER:
		case common::UI_EDP_WAITING_TO_STOP_READER:

			// jesli robot jest zsynchronizowany
			if (state.edp.is_synchronised) {

				switch (interface.mp->mp_state.state)
				{
					case common::UI_MP_NOT_PERMITED_TO_RUN:
					case common::UI_MP_PERMITED_TO_RUN:

						menu_Absolute_Moves->setEnabled(true);
						menu_Relative_Moves->setEnabled(true);
						menu_Tool->setEnabled(true);
						break;
					case common::UI_MP_WAITING_FOR_START_PULSE:

						menu_Absolute_Moves->setEnabled(true);
						menu_Relative_Moves->setEnabled(true);
						menu_Tool->setEnabled(true);

						break;
					case common::UI_MP_TASK_RUNNING:

						break;
					case common::UI_MP_TASK_PAUSED:

						menu_Absolute_Moves->setEnabled(false);
						menu_Relative_Moves->setEnabled(false);
						menu_Tool->setEnabled(false);
						break;
					default:
						break;
				}

			} else // jesli robot jest niezsynchronizowany
			{

				menu_Absolute_Moves->setEnabled(false);
				menu_Relative_Moves->setEnabled(false);
				menu_Tool->setEnabled(false);
			}
			break;
		default:
			break;

	}

}

void UiRobot::move_to_preset_position(int variant)
{

}

void UiRobot::setup_menubar()
{

	common_012::UiRobot::setup_menubar();
	Ui::MenuBar *menuBar = interface.get_main_window()->getMenuBar();
	Ui::SignalDispatcher *signalDispatcher = interface.get_main_window()->getSignalDispatcher();

	action_Pre_Synchro_Moves_Motors =
			new Ui::MenuBarAction(QString("&Motors"), wgts[WGT_MOTORS], signalDispatcher, menuBar);
	action_Absolute_Moves_Motors =
			new Ui::MenuBarAction(QString("&Motors"), wgts[WGT_MOTORS], signalDispatcher, menuBar);
	action_Joints = new Ui::MenuBarAction(QString("&Joints"), wgts[WGT_JOINTS], signalDispatcher, menuBar);
	action_Absolute_Moves_Xyz_Euler_Zyz =
			new Ui::MenuBarAction(QString("Xyz &Euler Zyz"), wgts[WGT_EULER], signalDispatcher, menuBar);
	action_Absolute_Moves_Xyz_Angle_Axis =
			new Ui::MenuBarAction(QString("Xyz &Angle Axis"), wgts[WGT_ANGLE_AXIS], signalDispatcher, menuBar);
	action_Xyz_Relative_Moves_Angle_Axis =
			new Ui::MenuBarAction(QString("Xyz &Angle Axis"), wgts[WGT_RELATIVE_ANGLE_AXIS], signalDispatcher, menuBar);
	action_Tool_Xyz_Euler_Zyz =
			new Ui::MenuBarAction(QString("Xyz &Euler Zyz"), wgts[WGT_TOOL_EULER], signalDispatcher, menuBar);
	action_Tool_Xyz_Angle_Axis =
			new Ui::MenuBarAction(QString("Xyz &Angle Axis"), wgts[WGT_TOOL_ANGLE_AXIS], signalDispatcher, menuBar);

	menu_Absolute_Moves = new QMenu(robot_menu);
	menu_Relative_Moves = new QMenu(robot_menu);
	menu_Tool = new QMenu(robot_menu);

	robot_menu->addAction(menu_Absolute_Moves->menuAction());
	robot_menu->addAction(menu_Relative_Moves->menuAction());
	robot_menu->addAction(menu_Tool->menuAction());

	menu_Pre_Synchro_Moves->addAction(action_Pre_Synchro_Moves_Motors);
	menu_Absolute_Moves->addAction(action_Absolute_Moves_Motors);
	menu_Absolute_Moves->addAction(action_Joints);
	menu_Absolute_Moves->addAction(action_Absolute_Moves_Xyz_Euler_Zyz);
	menu_Absolute_Moves->addAction(action_Absolute_Moves_Xyz_Angle_Axis);
	menu_Relative_Moves->addAction(action_Xyz_Relative_Moves_Angle_Axis);
	menu_Tool->addAction(action_Tool_Xyz_Euler_Zyz);
	menu_Tool->addAction(action_Tool_Xyz_Angle_Axis);

	robot_menu->setTitle(QApplication::translate("MainWindow", "Irp6&ot_m", 0, QApplication::UnicodeUTF8));
	menu_Absolute_Moves->setTitle(QApplication::translate("MainWindow", "A&bsolute moves", 0, QApplication::UnicodeUTF8));
	menu_Relative_Moves->setTitle(QApplication::translate("MainWindow", "Re&lative Moves", 0, QApplication::UnicodeUTF8));
	menu_Tool->setTitle(QApplication::translate("MainWindow", "&Tool", 0, QApplication::UnicodeUTF8));

}

}
} //namespace ui
} //namespace mrrocpp
