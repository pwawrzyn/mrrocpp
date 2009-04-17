#include <string.h>



#include "ecp/common/ecp_operator_reaction_condition.h"

namespace mrrocpp {
namespace ecp {
namespace common {

operator_reaction_condition::operator_reaction_condition (common::task::task& _ecp_task)
	:
	generator (_ecp_task)
{
	pose_list.clear();
	pose_list_iterator = pose_list.end();
	UI_fd = _ecp_task.UI_fd;
}

// destruktor
operator_reaction_condition::~operator_reaction_condition (void)
{
	flush_supplementary_list();
}

void operator_reaction_condition::flush_supplementary_list ( void )
{
	pose_list.clear();
}

void operator_reaction_condition::initiate_supplementary_list(void)
{
	pose_list_iterator = pose_list.begin();
}

void operator_reaction_condition::next_supplementary_list_ptr (void)
{
	if (pose_list_iterator != pose_list.end())
		pose_list_iterator++;
}

void operator_reaction_condition::get_supplementary (ecp_taught_in_pose& tip)
{
	tip.arm_type = pose_list_iterator->arm_type;
	tip.motion_time = pose_list_iterator->motion_time;
	memcpy(tip.coordinates, pose_list_iterator->coordinates, MAX_SERVOS_NR*sizeof(double));
}

void operator_reaction_condition::set_supplementary (lib::POSE_SPECIFICATION ps, double motion_time, double coordinates[MAX_SERVOS_NR])
{
	pose_list_iterator->arm_type = ps;
	pose_list_iterator->motion_time = motion_time;
	memcpy(pose_list_iterator->coordinates, coordinates, MAX_SERVOS_NR*sizeof(double));
}

void operator_reaction_condition::create_supplementary_list_head (lib::POSE_SPECIFICATION ps, double motion_time, double coordinates[MAX_SERVOS_NR])
{
	pose_list.push_back(ecp_taught_in_pose(ps, motion_time, coordinates));
	pose_list_iterator = pose_list.begin();
}

void operator_reaction_condition::insert_supplementary_list_element (lib::POSE_SPECIFICATION ps, double motion_time, double coordinates[MAX_SERVOS_NR])
{
	pose_list.push_back(ecp_taught_in_pose(ps, motion_time, coordinates));
	pose_list_iterator++;
}

bool operator_reaction_condition::is_supplementary_list_element ( void )
{
	// sprawdza czy aktualnie wskazywany jest element listy, czy lista sie skonczyla
	if ( pose_list_iterator != pose_list.end()) {
		return true;
	} else {
		return false;
	}
}

bool operator_reaction_condition::is_supplementary_list_head ( void )
{
	// sprawdza czy aktualnie wskazywany jest element listy, czy lista sie skonczyla
	if ( pose_list_iterator == pose_list.begin() ) {
		return true;
	} else {
		return false;
	}
}

int operator_reaction_condition::supplementary_list_length(void)
{
	return pose_list.size();
}

// --------------------------------------------------------------------------
bool operator_reaction_condition::first_step ()
{
	// bada wartosc warunku poczatkowego
	// true - konczy czekanie (funkcja wait)
	// false - kontynuuje oczekiwanie
	// Przy spelnieniu warunku wczytuje stan robota w zaleznosci od rozkazu przygotowanego w EDP_data

	// Stworzenie rozkazu odczytu wspolrzednych kartezjanskich
	communicate_with_edp = false; // wylaczamy robota na jeden krok

	return true;

} // end: irp6ot_operator_reaction_condition::condition_value ()
// --------------------------------------------------------------------------


// --------------------------------------------------------------------------
bool operator_reaction_condition::next_step ()
{
	// bada wartosc warunku poczatkowego
	// true - konczy czekanie (funkcja wait)
	// false - kontynuuje oczekiwanie
	// Przy spelnieniu warunku wczytuje stan robota w zaleznosci od rozkazu przygotowanego w EDP_data

	// Stworzenie rozkazu odczytu wspolrzednych kartezjanskich
	the_robot->EDP_data.instruction_type = lib::GET;
	the_robot->EDP_data.get_type = ARM_DV; // ARM
//  the_robot->EDP_data.get_arm_type = lib::XYZ_EULER_ZYZ;// W.S.
	the_robot->EDP_data.get_arm_type = lib::MOTOR;


	// Sprawdzenie warunku poczatkowego - reakcji operatora
	if (  ecp_t.operator_reaction ("Next motion?") ) {
		// Oczekiwanie skonczone
		// wyslanie rozkazu odczytu
		communicate_with_edp = true; // wlaczamy robota na jeden krok
		the_robot->execute_motion();
		communicate_with_edp = false; // wylaczamy robota
		// aktualizacja stanu the_robot na podstawie danych przyslanych z EDP

		if (!is_supplementary_list_head()) { // Czy wskaznik na glowe listy jest NULL
			// Tworzymy glowe listy
//      create_supplementary_list_head(lib::XYZ_EULER_ZYZ, 0.0, the_robot->EDP_data.current_XYZ_ZYZ_arm_coordinates); // W.S.
			create_supplementary_list_head(lib::MOTOR, 0.0, the_robot->EDP_data.current_motor_arm_coordinates);
		} else {
			// Wstaw do listy nowa pozycje
//      insert_supplementary_list_element(lib::XYZ_EULER_ZYZ, 0.0, the_robot->EDP_data.current_XYZ_ZYZ_arm_coordinates);
			insert_supplementary_list_element(lib::MOTOR, 0.0, the_robot->EDP_data.current_motor_arm_coordinates);
		}
		return false;
	} else {
		// Czekamy dalej
		return true;
	}
} // end: irp6ot_operator_reaction_condition::condition_value ()
// --------------------------------------------------------------------------
} // namespace common
} // namespace ecp
} // namespace mrrocpp

