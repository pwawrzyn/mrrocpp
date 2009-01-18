#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xinclude.h>
#include <libxml/xmlIO.h>
#include <libxml/xpath.h>

#include <gtk/gtktreestore.h>

#include <stdio.h>
#include <strings.h>
#include <string.h>

#include <cstdarg>

#include "configurator.h"
#include "ui_model.h"

const char *irp6[] = {"axis_xyz","axis_ts", "euler_xyz", "inc", "int", "servo"};
const char *irp6p[] = {"axis_xyz","euler_ts", "euler_xyz", "inc", "int", "servo"};

class configurator *config;

xmlXPathObjectPtr
configurator::getnodeset (xmlDocPtr doc, const xmlChar *xpath){

	xmlXPathContextPtr context;
	xmlXPathObjectPtr result;

	context = xmlXPathNewContext(doc);
	if (context == NULL) {
		printf("Error in xmlXPathNewContext\n");
		return NULL;
	}
	result = xmlXPathEvalExpression(xpath, context);
	xmlXPathFreeContext(context);
	if (result == NULL) {
		printf("Error in xmlXPathEvalExpression\n");
		return NULL;
	}
	if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
		xmlXPathFreeObject(result);
		return NULL;
	}
	return result;
}

void configurator::populate_tree_model_with_mp()
{
	xmlXPathObjectPtr mp = getnodeset(this->doc, (xmlChar *) "/config/mp[@active='true']");

	if (mp) {
		ui_config_entry & parent = ui_model::instance().add_ui_config_entry(ui_model::instance().getRootNode(), ui_config_entry::MP_PARENT, "Master Process");

		xmlNodeSetPtr nodeset = mp->nodesetval;
		for (int i = 0; i < nodeset->nodeNr; i++) {
			// get name attribute of the sensors
			xmlChar *mp_name = xmlGetProp(nodeset->nodeTab[i], (xmlChar *) "name");
			if (!mp_name) {
				g_warning("missing name for %s process", (char *) mp_name);
				continue;
			}

			char *ui_def = NULL;

			//! check in task specific config file
			ui_def = (ui_def) ? ui_def : this->get_string("/config/mp[@name='%s']/@ui_def", mp_name);
			ui_def = (ui_def) ? ui_def : this->get_string("/config/mp[@name='%s']/ui_def", mp_name);
			//! check in common config file
			ui_def = (ui_def) ? ui_def : this->get_string("/config/mp/@ui_def", mp_name);
			ui_def = (ui_def) ? ui_def : this->get_string("/config/mp/ui_def", mp_name);

			if(!ui_def) {
				g_error("missing ui_def for %s process", (char *) mp_name);
			}

			char *program_name = this->get_string("/config/mp[@name='%s']/program_name", mp_name);

			if (!program_name) {
				g_error("missing program name for %s process", (char *) mp_name);
				continue;
			}

			char *node_name = this->get_string("/config/mp[@name='%s']/node_name", mp_name);

			if (!node_name) {
				g_error("missing node name for %s process", (char *) mp_name);
				continue;
			}

			ui_model::instance().add_ui_config_entry(parent, ui_config_entry::MP, program_name, node_name, ui_def);
		}
		xmlXPathFreeObject(mp);
	}
}

void configurator::populate_tree_model_with_sensors()
{
	xmlXPathObjectPtr active_sensors = getnodeset(this->doc, (xmlChar *) "/config/sensors/sensor[@active='true']");

	if (active_sensors) {

		ui_config_entry & parent = ui_model::instance().add_ui_config_entry(ui_model::instance().getRootNode(), ui_config_entry::SENSORS_PARENT, "Sensors");

		xmlNodeSetPtr nodeset = active_sensors->nodesetval;
		for (int i = 0; i < nodeset->nodeNr; i++) {
			// get name attribute of the sensors
			xmlChar *sensor_name = xmlGetProp(nodeset->nodeTab[i], (xmlChar *) "name");
			if (!sensor_name)
				continue;

			char *program_name = this->get_string("/config/sensors/sensor[@name='%s']/vsp/program_name", sensor_name);
			if (!program_name) {
				g_error("missing program name for %s process", (char *) sensor_name);
				continue;
			}

			char *node_name = this->get_string("/config/sensors/sensor[@name='%s']/vsp/node_name", sensor_name);

			if (!node_name) {
				g_error("missing node name for %s process", (char *) sensor_name);
				continue;
			}

			ui_model::instance().add_ui_config_entry(parent, ui_config_entry::VSP, program_name, node_name);
		}
		xmlXPathFreeObject(active_sensors);
	}
}

void configurator::populate_tree_model_with_effectors()
{
	xmlXPathObjectPtr active_effectors = getnodeset(this->doc, (xmlChar *) "/config/effectors/effector[@active='true']");

	if (active_effectors) {

		ui_config_entry & parent = ui_model::instance().add_ui_config_entry(ui_model::instance().getRootNode(), ui_config_entry::EFFECTORS_PARENT, "Effectors");

		xmlNodeSetPtr nodeset = active_effectors->nodesetval;
		for (int i = 0; i < nodeset->nodeNr; i++) {
			// get name attribute of the sensors
			xmlChar *effector_name = xmlGetProp(nodeset->nodeTab[i], (xmlChar *) "name");
			if (!effector_name)
				continue;

			char *program_name = this->get_string("/config/effectors/effector[@name='%s']/ecp/program_name", effector_name);
			if (!program_name) {
				g_error("missing program name for %s process", (char *) effector_name);
				continue;
			}

			char *node_name = this->get_string("/config/effectors/effector[@name='%s']/ecp/node_name", effector_name);

			if (!node_name) {
				g_error("missing node name for %s process", (char *) effector_name);
				continue;
			}

			char *ui_def = NULL;

			ui_def = (ui_def) ? ui_def : this->get_string("/config/effectors/effector[@name='%s']/@ui_def", effector_name);
			ui_def = (ui_def) ? ui_def : this->get_string("/config/effectors/effector[@name='%s']/ui_def", effector_name);

			ui_config_entry & ecp_entry = ui_model::instance().add_ui_config_entry(parent, ui_config_entry::ECP, program_name, node_name, ui_def);

			program_name = this->get_string("/config/effectors/effector[@name='%s']/edp/program_name", effector_name);


			if (program_name && xmlStrEqual(effector_name, (xmlChar*)"irp6_mechatronika"))
			 {

				char *ui_def = NULL;
				program_name = this->get_string("/config/effectors/effector[@name='irp6_mechatronika']/edp[@name='edp_irp6m']/program_name");
				ui_def = (ui_def) ? ui_def : this->get_string("/config/effectors/effector[@name='irp6_mechatronika']/edp[@name='edp_irp6m']/@ui_def");
				ui_def = (ui_def) ? ui_def : this->get_string("/config/effectors/effector[@name='irp6_mechatronika']/edp[@name='edp_irp6m']/ui_def");
				ui_model::instance().add_ui_config_entry(ecp_entry, ui_config_entry::EDP, program_name, node_name, ui_def);

			}


			if (program_name && xmlStrEqual(effector_name, (xmlChar*)"irp6_ontrack"))
			 {

				char *ui_def = NULL;
				program_name = this->get_string("/config/effectors/effector[@name='irp6_ontrack']/edp[@name='edp_irp6o']/program_name");
				ui_def = (ui_def) ? ui_def : this->get_string("/config/effectors/effector[@name='irp6_ontrack']/edp[@name='edp_irp6o']/@ui_def");
				ui_def = (ui_def) ? ui_def : this->get_string("/config/effectors/effector[@name='irp6_ontrack']/edp[@name='edp_irp6o']/ui_def");
				ui_model::instance().add_ui_config_entry(ecp_entry, ui_config_entry::EDP, program_name, node_name, ui_def);

			}

			if (program_name && xmlStrEqual(effector_name, (xmlChar*)"irp6_postument"))
			 {

				char *ui_def = NULL;
				program_name = this->get_string("/config/effectors/effector[@name='irp6_postument']/edp[@name='edp_irp6p']/program_name");
				ui_def = (ui_def) ? ui_def : this->get_string("/config/effectors/effector[@name='irp6_postument']/edp[@name='edp_irp6p']/@ui_def");
				ui_def = (ui_def) ? ui_def : this->get_string("/config/effectors/effector[@name='irp6_postument']/edp[@name='edp_irp6p']/ui_def");
				ui_model::instance().add_ui_config_entry(ecp_entry, ui_config_entry::EDP, program_name, node_name, ui_def);

			}

			if (program_name && xmlStrEqual(effector_name, (xmlChar*)"conveyor"))
			 {

				char *ui_def = NULL;
				program_name = this->get_string("/config/effectors/effector[@name='conveyor']/edp[@name='edp_conveyor']/program_name");
				ui_def = (ui_def) ? ui_def : this->get_string("/config/effectors/effector[@name='conveyor']/edp[@name='edp_conveyor']/@ui_def");
				ui_def = (ui_def) ? ui_def : this->get_string("/config/effectors/effector[@name='conveyor']/edp[@name='edp_conveyor']/ui_def");
				ui_model::instance().add_ui_config_entry(ecp_entry, ui_config_entry::EDP, program_name, node_name, ui_def);

			}

		}
		xmlXPathFreeObject(active_effectors);
	}
}


void configurator::populate_tree_model()
{
	ui_model::instance().clear();
    populate_tree_model_with_sensors();
    populate_tree_model_with_effectors();
	populate_tree_model_with_mp();
}

configurator::configurator() : doc(NULL)
{
	// get the [configs/xml] subdirectory
	getcwd(config_dir, sizeof(config_dir));

	// go up 1 level
	char *ptr;
	for (int i = 0; i < 1; i++) {
		ptr = rindex(config_dir, '/');
		*ptr = '\0';
	}
	strcat(config_dir, "/configs/xml");
}

int configurator::open_config_file(const char *filename)
{
	// parse config file
	if (doc) {
		xmlFreeDoc(doc);
	}
	doc = xmlReadFile(filename, NULL, XML_PARSE_XINCLUDE);
	if (doc == NULL) {
		fprintf(stderr, "failed to parse the including file\n");
		exit(1);
	}

	// apply the XInclude process
	if (xmlXIncludeProcess(doc) < 0) {
		fprintf(stderr, "XInclude processing failed\n");
		exit(1);
	}

	this->populate_tree_model();

	return 0;
}

configurator::~configurator()
{
	// free the document
	if (doc) {
		xmlFreeDoc(doc);
	}

	// Cleanup function for the XML library.
	xmlCleanupParser();
}

char * configurator::get_string(const char *xpath, ...)
{
	va_list ap;

	va_start(ap, xpath);
	xmlChar query[256];
	vsprintf((char *) query, xpath, ap);
	va_end(ap);

	xmlXPathObjectPtr result = getnodeset(this->doc, query);

	if (!result) return NULL;
	if (result->type != XPATH_NODESET) {
		xmlXPathFreeObject(result);
		return NULL;
	}

	char *ret = (char *) xmlNodeListGetString(this->doc, result->nodesetval->nodeTab[0]->children, 1);
	xmlXPathFreeObject(result);
	return ret;
}
