#include "c74_msp.h"

#include "elements/dsp/part.h"

using namespace c74::max;

static t_class* this_class = nullptr;

struct t_cosas {
	t_pxobject x_obj;

	double param_contour;
	double param_bow;
	double param_blow;
	double param_strike;
	double param_coarse;
	double param_fine;
	double param_fm;
	double param_flow;
	double param_mallet;
	double param_geometry;
	double param_brightness;
	double param_bow_timbre;
	double param_blow_timbre;
	double param_strike_timbre;
	double param_damping;
	double param_position;
	double param_space;
	double param_play;
	double param_note_input;

	t_cosas();
	void init();
	void step();
	void changeRate();

	long sf = 0;
	long sr = 48000.0;

	float* outbuf;
	float* auxbuf;
	float* blow;
	float* strike;
	float* silence;

	elements::Part* part;
	uint16_t reverb_buffer[32768] = {};

};

t_cosas::t_cosas() {
}

void t_cosas::init() {

	param_contour = 1.;
	param_bow = 0.;
	param_blow = 0.;
	param_strike = 0.5;
	param_coarse = 0.;
	param_fine = 0.;
	param_fm = 0.;
	param_flow = 0.5;
	param_mallet = 0.5;
	param_geometry = 0.5;
	param_brightness = 0.5;
	param_bow_timbre = 0.5;
	param_blow_timbre = 0.5;
	param_strike_timbre = 0.5;
	param_damping = 0.5;
	param_position = 0.5;
	param_space = 0.;
	param_play = 0.;
	param_note_input = 0.;

	part = new elements::Part();
	// In the Mutable Instruments code, Part doesn't initialize itself, so zero it here.
	memset(part, 0, sizeof(*part));
	part->Init(reverb_buffer);
	// Just some random numbers
	uint32_t seed[3] = {1, 2, 3};
	part->Seed(seed, 3);
	changeRate();

}

void t_cosas::changeRate() {
	outbuf = new float[sf];
	auxbuf = new float[sf];
	blow = new float[sf];
	strike = new float[sf];
	silence = new float[sf];
  	std::fill(&silence[0], &silence[sf], 0.0f);
}

void t_cosas::step() {

	elements::Patch* p = part->mutable_patch();
	p->exciter_envelope_shape = param_contour;
	p->exciter_bow_level = param_bow;
	p->exciter_blow_level = param_blow;
	p->exciter_strike_level = param_strike;
	p->exciter_bow_timbre = param_bow_timbre;
	p->exciter_blow_meta = param_flow;
	p->exciter_blow_timbre = param_blow;
	p->exciter_strike_meta = param_mallet;
	p->exciter_strike_timbre = param_strike_timbre;
	p->resonator_geometry = param_geometry;
	p->resonator_brightness = param_brightness;
	p->resonator_damping = param_damping;
	p->resonator_position = param_position;
	p->space = param_space;

	elements::PerformanceState performance;
	performance.note = 12.0f * param_note_input + param_coarse + param_fine;
	performance.modulation = 0.;
	performance.gate = param_play >= 1.0;
	performance.strength = 0.5f;


	part->Process(performance, silence, silence, outbuf, auxbuf, sf);

}

void* cosas_new(void) {

	t_cosas* self = (t_cosas*) object_alloc(this_class);

	dsp_setup((t_pxobject*)self, 0);
	outlet_new(self, "signal");
	outlet_new(self, "signal");

	self->x_obj.z_misc = Z_NO_INPLACE;
	self->init();

	return (void *)self;
}


void cosas_perform64(t_cosas* self, t_object* dsp64, double** ins, long numins, double** outs, long numouts, long sampleframes, long flags, void* userparam) {

    double    *out = outs[0];
    double    *out2 = outs[1];

	if (self->sf!=sampleframes){
		self->sf=sampleframes;
		self->changeRate();
	}

	self->step();

	for (int i = 0; i < sampleframes; i++) {
		*out++ = self->outbuf[i];
		*out2++ = self->auxbuf[i];
	}

}

void cosas_free(t_cosas* self) {
	dsp_free((t_pxobject*)self);
}

void cosas_dsp64(t_cosas* self, t_object* dsp64, short* count, double samplerate, long maxvectorsize, long flags) {
	object_method_direct(void, (t_object*, t_object*, t_perfroutine64, long, void*),
						 dsp64, gensym("dsp_add64"), (t_object*)self, (t_perfroutine64)cosas_perform64, 0, NULL);

	if (self->sr!=samplerate)
	{
		self->sr=samplerate;
		self->changeRate();
	}
}


void cosas_assist(t_cosas* self, void* unused, t_assist_function io, long index, char* string_dest) {
	if (io == ASSIST_INLET) {
		switch (index) {
			case 0: 
				strncpy(string_dest,"(parameters) IN", ASSIST_STRING_MAXSIZE); 
				break;
		}
	}
	else if (io == ASSIST_OUTLET) {
		switch (index) {
			case 0: 
				strncpy(string_dest,"(signal) L", ASSIST_STRING_MAXSIZE); 
				break;
			case 1: 
				strncpy(string_dest,"(signal) R", ASSIST_STRING_MAXSIZE); 
				break;
		}
	}
}

void cosas_contour(t_cosas *x, double f){
	x->param_contour = f;
}

void cosas_bow(t_cosas *x, double f){
	x->param_bow = f;
}

void cosas_blow(t_cosas *x, double f){
	x->param_blow = f;
}

void cosas_strike(t_cosas *x, double f){
	x->param_strike = f;
}

void cosas_note_input(t_cosas *x, double f){
	x->param_note_input = f;
}

void cosas_coarse(t_cosas *x, double f){
	x->param_coarse = f;
}

void cosas_fine(t_cosas *x, double f){
	x->param_fine = f;
}

void cosas_fm(t_cosas *x, double f){
	x->param_fm = f;
}

void cosas_flow(t_cosas *x, double f){
	x->param_flow = f;
}

void cosas_mallet(t_cosas *x, double f){
	x->param_mallet = f;
}

void cosas_geometry(t_cosas *x, double f){
	x->param_geometry = f;
}

void cosas_brightness(t_cosas *x, double f){
	x->param_brightness = f;
}

void cosas_bow_timbre(t_cosas *x, double f){
	x->param_bow_timbre = f;
}

void cosas_blow_timbre(t_cosas *x, double f){
	x->param_blow_timbre = f;
}

void cosas_strike_timbre(t_cosas *x, double f){
	x->param_strike_timbre = f;
}

void cosas_damping(t_cosas *x, double f){
	x->param_damping = f;
}

void cosas_position(t_cosas *x, double f){
	x->param_position = f;
}

void cosas_space(t_cosas *x, double f){
	x->param_space = f;
}

void cosas_play(t_cosas *x, double f){
	x->param_play = f;
}


void ext_main(void* r) {
	this_class = class_new("cosas~", (method)cosas_new, (method)cosas_free, sizeof(t_cosas), NULL, A_GIMME, 0);

	class_addmethod(this_class,(method) cosas_dsp64, "dsp64",	A_CANT,		0);

	class_addmethod(this_class,(method) cosas_contour,"contour",A_DEFFLOAT,0);
	class_addmethod(this_class,(method) cosas_bow,"bow",A_DEFFLOAT,0);
	class_addmethod(this_class,(method) cosas_blow,"blow",A_DEFFLOAT,0);
	class_addmethod(this_class,(method) cosas_strike,"strike",A_DEFFLOAT,0);
	class_addmethod(this_class,(method) cosas_note_input,"note_input",A_DEFFLOAT,0);
	class_addmethod(this_class,(method) cosas_coarse,"coarse",A_DEFFLOAT,0);
	class_addmethod(this_class,(method) cosas_fine,"fine",A_DEFFLOAT,0);
	class_addmethod(this_class,(method) cosas_fm,"fm",A_DEFFLOAT,0);
	class_addmethod(this_class,(method) cosas_flow,"flow",A_DEFFLOAT,0);
	class_addmethod(this_class,(method) cosas_mallet,"mallet",A_DEFFLOAT,0);
	class_addmethod(this_class,(method) cosas_geometry,"geometry",A_DEFFLOAT,0);
	class_addmethod(this_class,(method) cosas_brightness,"brightness",A_DEFFLOAT,0);
	class_addmethod(this_class,(method) cosas_bow_timbre,"bow_timbre",A_DEFFLOAT,0);
	class_addmethod(this_class,(method) cosas_blow_timbre,"blow_timbre",A_DEFFLOAT,0);
	class_addmethod(this_class,(method) cosas_strike_timbre,"strike_timbre",A_DEFFLOAT,0);
	class_addmethod(this_class,(method) cosas_damping,"damping",A_DEFFLOAT,0);
	class_addmethod(this_class,(method) cosas_position,"position",A_DEFFLOAT,0);
	class_addmethod(this_class,(method) cosas_space,"space",A_DEFFLOAT,0);
	class_addmethod(this_class,(method) cosas_play,"play",A_DEFFLOAT,0);

	class_addmethod(this_class,(method) cosas_assist, "assist",	A_CANT,		0);

	class_dspinit(this_class);
	class_register(CLASS_BOX, this_class);
}