#ifndef __UI_R_SBENCH_H
#define __UI_R_SBENCH_H

/*!
 * @file
 * @brief File contains UiRobot class declaration for SwarmItFix Bench
 * @author twiniars <twiniars@ia.pw.edu.pl>, Warsaw University of Technology
 *
 * @ingroup sbench
 */

#include <QObject>
#include <QMenu>
#include "../base/ui.h"
#include "../base/ui_robot.h"

#include "wgt_sbench_voltage_command.h"
#include "wgt_sbench_preasure_command.h"

namespace Ui {
class MenuBar;
class MenuBarAction;
}

namespace mrrocpp {
namespace ui {
namespace common {
class Interface;
}

namespace sbench {

class EcpRobot;
//
//
// KLASA UiRobot
//
//

class UiRobot : public common::UiRobot
{
	Q_OBJECT

public:

	EcpRobot *ui_ecp_robot;

	UiRobot(common::Interface& _interface);
	void create_ui_ecp_robot();
	void manage_interface();
	void delete_ui_ecp_robot();
	void null_ui_ecp_robot();
	void synchronise();

	int ui_get_edp_pid();
	void ui_get_controler_state(lib::controller_state_t & robot_controller_initial_state_l);

	void setup_menubar();

private:
	QAction *action_voltage_command;
	QAction *action_preasure_command;

};

}
} //namespace ui
} //namespace mrrocpp

#endif

