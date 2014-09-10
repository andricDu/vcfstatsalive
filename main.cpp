#include <iostream>
#include <getopt.h>

#include <string>
#include <fstream>

#include "Variant.h"

#include "DummyStatsCollector.h"
#include "BasicStatsCollector.h"


using namespace std;
using namespace VcfStatsAlive;

static struct option getopt_options[] =
{
	/* These options set a flag. */
	//{"verbose", no_argument,       &verbose_flag, 1},
	//{"brief",   no_argument,       &verbose_flag, 0},
	/* These options don't set a flag.
	 * We distinguish them by their indices. */
	{"update-rate",		required_argument,	0, 'u'},
	{"first-update",	required_argument,	0, 'f'},
	{0, 0, 0, 0}
};

static unsigned int updateRate;
static unsigned int firstUpdateRate;

void printStatsJansson(AbstractStatCollector* rootStatCollector);

int main(int argc, char* argv[]) {

	string filename;
	updateRate = 1000;
	firstUpdateRate = 0;

	int option_index = 0;

	int ch;
	while((ch = getopt_long (argc, argv, "f:u:", getopt_options, &option_index)) != -1) {
		switch(ch) {
			case 0:
				break;
			case 'u':
				updateRate = strtol(optarg, NULL, 10);
				break;
			case 'f':
				firstUpdateRate = strtol(optarg, NULL, 10);
				break;
			break;
		}
	}

	argc -= optind;
	argv += optind;

	vcf::VariantCallFile vcfFile;

	std::istream* fin;


	if (argc == 0) {
		fin = &cin;
	}
	else {
		fin = new ifstream(*argv);
	}

	vcfFile.open(*fin);

	if(!vcfFile.is_open()) {
		std::cerr<<"Unable to open vcf file / stream"<<std::endl;
		exit(1);
	}

	DummyStatsCollector *root = new DummyStatsCollector();
	root->addChild(new BasicStatsCollector());

	vcf::Variant var(vcfFile);

	while(vcfFile.is_open() && !vcfFile.done()) {
		vcfFile.getNextVariant(var);
		root->processVariant(var);
	}

	if(fin != &cin) delete fin;

	printStatsJansson(root);

	return 0;
}

void printStatsJansson(AbstractStatCollector* rootStatCollector) {

	// Create the root object that contains everything
	json_t * j_root = json_object();

	// Let the root object of the collector tree create Json
	rootStatCollector->appendJson(j_root);

	// Dump the json
	cout<<json_dumps(j_root, JSON_COMPACT | JSON_ENSURE_ASCII | JSON_PRESERVE_ORDER)<<endl;
}
