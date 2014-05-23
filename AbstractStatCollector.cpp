#include "AbstractStatCollector.h"

using namespace VcfStatsAlive;

AbstractStatCollector::AbstractStatCollector(const std::string* sampleName = NULL) {
	_children.clear(), _sampleName(sampleName);
}

AbstractStatCollector::~AbstractStatCollector() {

}

void AbstractStatCollector::addChild(AbstractStatCollector *child) {

	// Make sure that the input is good
	if(child == NULL) return;

	// Make sure the input is not yet a child
	StatCollectorPtrVec::const_iterator loc = std::find(_children.begin(), _children.end(), child);
	if(loc != _children.end()) return;

	// Insert the input to the end of the children list
	_children.push_back(child);
}

void AbstractStatCollector::removeChild(AbstractStatCollector * child) {

	// Make sure that the input is good
	if(child == NULL) return;

	// Make sure the input is a child
	StatCollectorPtrVec::iterator loc = std::find(_children.begin(), _children.end(), child);
	if(loc == _children.end()) return;

	_children.erase(loc);
}

void AbstractStatCollector::processVariant(const vcf::Variant& var) {

	if(_sampleName == NULL || var.sampleName.compare(*_sampleName) == 0) {
		this->processVariantImpl(var);
	}

	StatCollectorPtrVec::iterator iter;
	for(iter = _children.begin(); iter != _children.end(); iter++) {
		(*iter)->processVariant(var);
	}
}

json_t * AbstractStatCollector::appendJson(json_t * jsonRootObj) {
	if(jsonRootObj == NULL)
		jsonRootObj = json_object();

	this->appendJsonImpl(jsonRootObj);
	
	StatCollectorPtrVec::iterator iter;
	for(iter = _children.begin(); iter != _children.end(); iter++) {
		(*iter)->appendJson(jsonRootObj);
	}

	return jsonRootObj;
}

bool AbstractStatCollector::isSatisfiedImpl() {
	return false;
}

bool AbstractStatCollector::isSatisfied() {
	if (not this->isSatisfiedImpl()) return false;

	bool isChildrenSatisfied = true;

	StatCollectorPtrVec::iterator iter;
	for(iter = _children.begin(); iter != _children.end(); iter++) {
		isChildrenSatisfied = isChildrenSatisfied && (*iter)->isSatisfied();
	}

	return isChildrenSatisfied;
}
