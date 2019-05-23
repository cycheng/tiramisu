#include <tiramisu/tiramisu.h>
#include <string.h>
#include "baryon_wrapper.h"

using namespace tiramisu;

/**
  * Multiply the two complex numbers p1 and p2 and return the real part.
  */
expr mul_r(std::pair<expr, expr> p1, std::pair<expr, expr> p2)
{
    expr e1 = (p1.first * p2.first);
    expr e2 = (p1.second * p2.second);
    return (e1 - e2);
}

/**
  * Multiply the two complex numbers p1 and p2 and return the imaginary part.
  */
expr mul_i(std::pair<expr, expr> p1, std::pair<expr, expr> p2)
{
    expr e1 = (p1.first * p2.second);
    expr e2 = (p1.second * p2.first);
    return (e1 + e2);
}

/*
 * The goal is to generate code that implements the reference.
 * baryon_ref.cpp
 */
void generate_function(std::string name)
{
    tiramisu::init(name);

    var n("n", 0, Nsrc),
	iCprime("iCprime", 0, Nc),
	iSprime("iSprime", 0, Ns),
	jCprime("jCprime", 0, Nc),
	jSprime("jSprime", 0, Ns),
	kCprime("kCprime", 0, Nc),
	kSprime("kSprime", 0, Ns),
	lCprime("lCprime", 0, Nc),
	lSprime("lSprime", 0, Ns),
	x("x", 0, Vsnk),
	x2("x2", 0, Vsnk),
	t("t", 0, Lt),
	wnum("wnum", 0, Nw),
	y("y", 0, Vsrc),
	tri("tri", 0, Nq);

    input Blocal_r("Blocal_r", {n, iCprime, iSprime, jCprime, jSprime, kCprime, kSprime, x, t}, p_float64);
    input Blocal_i("Blocal_i", {n, iCprime, iSprime, jCprime, jSprime, kCprime, kSprime, x, t}, p_float64);
    input   prop_r("prop_r",   {tri, iCprime, iSprime, jCprime, jSprime, x, t, y}, p_float64);
    input   prop_i("prop_i",   {tri, iCprime, iSprime, jCprime, jSprime, x, t, y}, p_float64);
    input  weights("weights",  {wnum}, p_float64);
    input    psi_r("psi_r",    {n, y}, p_float64);
    input    psi_i("psi_i",    {n, y}, p_float64);
    input    color_weights("color_weights",    {wnum, tri}, p_int32);
    input    spin_weights("spin_weights",    {wnum, tri}, p_int32);

    computation Blocal_r_init("Blocal_r_init", {n, iCprime, iSprime, jCprime, jSprime, kCprime, kSprime, x, t}, expr((double) 0));
    computation Blocal_i_init("Blocal_i_init", {n, iCprime, iSprime, jCprime, jSprime, kCprime, kSprime, x, t}, expr((double) 0));

    computation iC("iC", {wnum}, color_weights(wnum, 0));
    computation iS("iS", {wnum}, spin_weights(wnum, 0));
    computation jC("jC", {wnum}, color_weights(wnum, 1));
    computation jS("jS", {wnum}, spin_weights(wnum, 1));
    computation kC("kC", {wnum}, color_weights(wnum, 2));
    computation kS("kS", {wnum}, spin_weights(wnum, 2));

    std::pair<expr, expr> prop_0(prop_r(0, iCprime, iSprime, iC(wnum), iS(wnum), x, t, y), prop_i(0, iCprime, iSprime, iC(wnum), iS(wnum), x, t, y));
    std::pair<expr, expr> prop_2(prop_r(2, kCprime, kSprime, kC(wnum), kS(wnum), x, t, y), prop_i(2, kCprime, kSprime, kC(wnum), kS(wnum), x, t, y));
    std::pair<expr, expr> prop_0p(prop_r(0, kCprime, kSprime, iC(wnum), iS(wnum), x, t, y), prop_i(0, kCprime, kSprime, iC(wnum), iS(wnum), x, t, y));
    std::pair<expr, expr> prop_2p(prop_r(2, iCprime, iSprime, kC(wnum), kS(wnum), x, t, y), prop_i(2, iCprime, iSprime, kC(wnum), kS(wnum), x, t, y));
    std::pair<expr, expr> m1(mul_r(prop_0, prop_2) - mul_r(prop_0p, prop_2p), mul_i(prop_0, prop_2) - mul_i(prop_0p, prop_2p));
    std::pair<expr, expr> psi(psi_r(n, y), psi_i(n, y));
    std::pair<expr, expr> m2(mul_r(psi, m1), mul_i(psi, m1));
    expr prop_r_1 = prop_r(1, jCprime, jSprime, jC(wnum), jS(wnum), x, t, y);
    expr prop_i_1 = prop_i(1, jCprime, jSprime, jC(wnum), jS(wnum), x, t, y);
    std::pair<expr, expr> prop_1(prop_r_1, prop_i_1);

    computation Blocal_r_update("Blocal_r_update", {wnum, n, iCprime, iSprime, jCprime, jSprime, kCprime, kSprime, x, t, y}, p_float64);
    Blocal_r_update.set_expression(Blocal_r_init(n, iCprime, iSprime, jCprime, jSprime, kCprime, kSprime, x, t) + weights(wnum) * mul_r(m2, prop_1));

    computation Blocal_i_update("Blocal_i_update", {wnum, n, iCprime, iSprime, jCprime, jSprime, kCprime, kSprime, x, t, y}, p_float64);
    Blocal_i_update.set_expression(Blocal_i_init(n, iCprime, iSprime, jCprime, jSprime, kCprime, kSprime, x, t) + weights(wnum) * mul_i(m2, prop_1));

    computation Q_r_init("Q_r_init", {n, iCprime, iSprime, kCprime, kSprime, jCprime, jSprime, x, t, y}, expr((double) 0));
    computation Q_i_init("Q_i_init", {n, iCprime, iSprime, kCprime, kSprime, jCprime, jSprime, x, t, y}, expr((double) 0));

    computation Bsingle_r_init("Bsingle_r_init", {n, iCprime, iSprime, jCprime, jSprime, kCprime, kSprime, x, x2, t}, expr((double) 0));
    computation Bsingle_i_init("Bsingle_i_init", {n, iCprime, iSprime, jCprime, jSprime, kCprime, kSprime, x, x2, t}, expr((double) 0));

    computation Q_r_update("Q_r_update", {wnum, n, iCprime, iSprime, kCprime, kSprime, jCprime, jSprime, x, t, y},
			Q_r_init(n, iCprime, iSprime, kCprime, kSprime, jC(wnum), jS(wnum), x, t, y) + weights(wnum) * mul_r(psi, m1));
    Q_r_update.add_predicate((jCprime == jC(wnum)) && (jSprime == jS(wnum)));

    computation Q_i_update("Q_i_update", {wnum, n, iCprime, iSprime, kCprime, kSprime, jCprime, jSprime, x, t, y},
			Q_i_init(n, iCprime, iSprime, kCprime, kSprime, jC(wnum), jS(wnum), x, t, y) + weights(wnum) * mul_i(psi, m1));
    Q_i_update.add_predicate((jCprime == jC(wnum)) && (jSprime == jS(wnum)));

    std::pair<expr, expr> Q_update(Q_r_update(0, n, iCprime, iSprime, kCprime, kSprime, lCprime, lSprime, x, t, y), Q_i_update(0, n, iCprime, iSprime, kCprime, kSprime, lCprime, lSprime, x, t, y));
    std::pair<expr, expr> prop_1p(prop_r(1, jCprime, jSprime, lCprime, lSprime, x2, t, y), prop_i(1, jCprime, jSprime, lCprime, lSprime, x2, t, y));

    computation Bsingle_r_update("Bsingle_r_update", {n, iCprime, iSprime, jCprime, jSprime, kCprime, kSprime, lCprime, lSprime, x, x2, t, y},
	    Bsingle_r_init(n, iCprime, iSprime, jCprime, jSprime, kCprime, kSprime, x, x2, t) + mul_r(Q_update, prop_1p));

    computation Bsingle_i_update("Bsingle_i_update", {n, iCprime, iSprime, jCprime, jSprime, kCprime, kSprime, lCprime, lSprime, x, x2, t, y},
	    Bsingle_i_init(n, iCprime, iSprime, jCprime, jSprime, kCprime, kSprime, x, x2, t) + mul_i(Q_update, prop_1p));

    computation Bdouble_r_init("Bdouble_r_init", {n, iCprime, iSprime, jCprime, jSprime, kCprime, kSprime, x, x2, t}, expr((double) 0));
    computation Bdouble_i_init("Bdouble_i_init", {n, iCprime, iSprime, jCprime, jSprime, kCprime, kSprime, x, x2, t}, expr((double) 0));

    computation O_r_init("O_r_init", {n, jCprime, jSprime, kCprime, kSprime, iCprime, iSprime, x, t, y}, expr((double) 0));
    computation O_i_init("O_i_init", {n, jCprime, jSprime, kCprime, kSprime, iCprime, iSprime, x, t, y}, expr((double) 0));

    computation P_r_init("P_r_init", {n, jCprime, jSprime, kCprime, kSprime, iCprime, iSprime, x, t, y}, expr((double) 0));
    computation P_i_init("P_i_init", {n, jCprime, jSprime, kCprime, kSprime, iCprime, iSprime, x, t, y}, expr((double) 0));

    std::pair<expr, expr> m3(mul_r(psi, prop_1), mul_i(psi, prop_1));
    computation O_r_update("O_r_update", {wnum, n, jCprime, jSprime, kCprime, kSprime, iCprime, iSprime, x, t, y},
			O_r_init(n, jCprime, jSprime, kCprime, kSprime, iC(wnum), iS(wnum), x, t, y) + weights(wnum) * mul_r(m3, prop_2));
    O_r_update.add_predicate((iCprime == iC(wnum)) && (iSprime == iS(wnum)));

    computation O_i_update("O_i_update", {wnum, n, jCprime, jSprime, kCprime, kSprime, iCprime, iSprime, x, t, y},
			O_i_init(n, jCprime, jSprime, kCprime, kSprime, iC(wnum), iS(wnum), x, t, y) + weights(wnum) * mul_i(m3, prop_2));
    O_i_update.add_predicate((iCprime == iC(wnum)) && (iSprime == iS(wnum)));

    std::pair<expr, expr> m4(mul_r(psi, prop_0p), mul_i(psi, prop_0p));
    computation P_r_update("P_r_update", {wnum, n, jCprime, jSprime, kCprime, kSprime, iCprime, iSprime, x, t, y},
			P_r_init(n, jCprime, jSprime, kCprime, kSprime, kC(wnum), kS(wnum), x, t, y) + weights(wnum) * mul_r(m4, prop_1));
    P_r_update.add_predicate((iCprime == kC(wnum)) && (iSprime == kS(wnum)));

    computation P_i_update("P_i_update", {wnum, n, jCprime, jSprime, kCprime, kSprime, iCprime, iSprime, x, t, y},
			P_i_init(n, jCprime, jSprime, kCprime, kSprime, kC(wnum), kS(wnum), x, t, y) + weights(wnum) * mul_i(m4, prop_1));
    P_i_update.add_predicate((iCprime == kC(wnum)) && (iSprime == kS(wnum)));

    std::pair<expr, expr> O_update(O_r_update(0, n, jCprime, jSprime, kCprime, kSprime, lCprime, lSprime, x, t, y), O_i_update(0, n, jCprime, jSprime, kCprime, kSprime, lCprime, lSprime, x, t, y));
    std::pair<expr, expr> P_update(P_r_update(0, n, jCprime, jSprime, kCprime, kSprime, lCprime, lSprime, x, t, y), P_i_update(0, n, jCprime, jSprime, kCprime, kSprime, lCprime, lSprime, x, t, y));
    std::pair<expr, expr> prop_0pp(prop_r(0, iCprime, iSprime, lCprime, lSprime, x2, t, y), prop_i(0, iCprime, iSprime, lCprime, lSprime, x2, t, y));

    computation Bdouble_r_update0("Bdouble_r_update0", {n, iCprime, iSprime, jCprime, jSprime, kCprime, kSprime, lCprime, lSprime, x, x2, t, y},
	    Bdouble_r_init(n, iCprime, iSprime, jCprime, jSprime, kCprime, kSprime, x, x2, t) + mul_r(prop_0pp, O_update));

    computation Bdouble_i_update0("Bdouble_i_update0", {n, iCprime, iSprime, jCprime, jSprime, kCprime, kSprime, lCprime, lSprime, x, x2, t, y},
	    Bdouble_i_init(n, iCprime, iSprime, jCprime, jSprime, kCprime, kSprime, x, x2, t) + mul_i(prop_0pp, O_update));

    std::pair<expr, expr> prop_2pp(prop_r(2, iCprime, iSprime, lCprime, lSprime, x2, t, y), prop_i(2, iCprime, iSprime, lCprime, lSprime, x2, t, y));
    computation Bdouble_r_update1("Bdouble_r_update1", {n, iCprime, iSprime, jCprime, jSprime, kCprime, kSprime, lCprime, lSprime, x, x2, t, y},
	    Bdouble_r_init(n, iCprime, iSprime, jCprime, jSprime, kCprime, kSprime, x, x2, t) - mul_r(P_update, prop_2pp));

    computation Bdouble_i_update1("Bdouble_i_update1", {n, iCprime, iSprime, jCprime, jSprime, kCprime, kSprime, lCprime, lSprime, x, x2, t, y},
	    Bdouble_i_init(n, iCprime, iSprime, jCprime, jSprime, kCprime, kSprime, x, x2, t) - mul_i(P_update, prop_2pp));

    // -------------------------------------------------------
    // Layer II
    // -------------------------------------------------------
    Blocal_r_init.then(Blocal_i_init, t)
		 .then(Q_r_init, t)
		 .then(Q_i_init, y)
		 .then(Bsingle_r_init, computation::root)
		 .then(Bsingle_i_init, t)
		 .then(Bdouble_r_init, t)
		 .then(Bdouble_i_init, t)
		 .then(O_r_init, computation::root)
		 .then(O_i_init, y)
		 .then(P_r_init, y)
		 .then(P_i_init, y)
	         .then(iC, computation::root)
		 .then(iS, wnum)
		 .then(jC, wnum)
		 .then(jS, wnum)
		 .then(kC, wnum)
		 .then(kS, wnum)
		 .then(Blocal_r_update, wnum)
		 .then(Blocal_i_update, y)
		 .then(Q_r_update, y)
		 .then(Q_i_update, y)
		 .then(O_r_update, y)
		 .then(O_i_update, y)
		 .then(P_r_update, y)
		 .then(P_i_update, y)
		 .then(Bsingle_r_update, computation::root)
		 .then(Bsingle_i_update, y)
		 .then(Bdouble_r_update0, y)
		 .then(Bdouble_i_update0, y)
		 .then(Bdouble_r_update1, y)
		 .then(Bdouble_i_update1, y);


    Blocal_r_init.tag_parallel_level(n);
    Bsingle_r_init.tag_parallel_level(n);
    O_r_init.tag_parallel_level(n);

    Blocal_r_init.vectorize(t, Lt);
    Bsingle_r_init.vectorize(t, Lt);
    O_r_init.vectorize(y, Vsrc);

    Blocal_r_update.tag_parallel_level(n);
    Blocal_r_update.vectorize(t, Lt);
    //Blocal_r_update.unroll(y, Vsrc);
    //Blocal_r_update.unroll(x, Vsnk);

    Q_r_update.vectorize(y, Vsrc);

    Bsingle_r_update.tag_parallel_level(n);
    Bsingle_r_update.vectorize(t, Lt);

    // -------------------------------------------------------
    // Layer III
    // -------------------------------------------------------
    buffer buf_Blocal_r("buf_Blocal_r", {Nsrc, Nc, Ns, Nc, Ns, Nc, Ns, Vsnk, Lt}, p_float64, a_output);
    buffer buf_Blocal_i("buf_Blocal_i", {Nsrc, Nc, Ns, Nc, Ns, Nc, Ns, Vsnk, Lt}, p_float64, a_output);
    buffer buf_Q_r("buf_Q_r", {Nsrc, Nc, Ns, Nc, Ns, Nc, Ns, Vsnk, Lt, Vsnk}, p_float64, a_output);
    buffer buf_Q_i("buf_Q_i", {Nsrc, Nc, Ns, Nc, Ns, Nc, Ns, Vsnk, Lt, Vsnk}, p_float64, a_output);
    buffer buf_O_r("buf_O_r", {Nsrc, Nc, Ns, Nc, Ns, Nc, Ns, Vsnk, Lt, Vsnk}, p_float64, a_output);
    buffer buf_O_i("buf_O_i", {Nsrc, Nc, Ns, Nc, Ns, Nc, Ns, Vsnk, Lt, Vsnk}, p_float64, a_output);
    buffer buf_P_r("buf_P_r", {Nsrc, Nc, Ns, Nc, Ns, Nc, Ns, Vsnk, Lt, Vsnk}, p_float64, a_output);
    buffer buf_P_i("buf_P_i", {Nsrc, Nc, Ns, Nc, Ns, Nc, Ns, Vsnk, Lt, Vsnk}, p_float64, a_output);
    buffer buf_Bsingle_r("buf_Bsingle_r", {Nsrc, Nc, Ns, Nc, Ns, Nc, Ns, Vsnk, Vsnk, Lt}, p_float64, a_output);
    buffer buf_Bsingle_i("buf_Bsingle_i", {Nsrc, Nc, Ns, Nc, Ns, Nc, Ns, Vsnk, Vsnk, Lt}, p_float64, a_output);
    buffer buf_Bdouble_r("buf_Bdouble_r", {Nsrc, Nc, Ns, Nc, Ns, Nc, Ns, Vsnk, Vsnk, Lt}, p_float64, a_output);
    buffer buf_Bdouble_i("buf_Bdouble_i", {Nsrc, Nc, Ns, Nc, Ns, Nc, Ns, Vsnk, Vsnk, Lt}, p_float64, a_output);

    Blocal_r.store_in(&buf_Blocal_r);
    Blocal_i.store_in(&buf_Blocal_i);
    Blocal_r_init.store_in(&buf_Blocal_r);
    Blocal_i_init.store_in(&buf_Blocal_i);
    Blocal_r_update.store_in(&buf_Blocal_r, {n, iCprime, iSprime, jCprime, jSprime, kCprime, kSprime, x, t});
    Blocal_i_update.store_in(&buf_Blocal_i, {n, iCprime, iSprime, jCprime, jSprime, kCprime, kSprime, x, t});

    Q_r_init.store_in(&buf_Q_r);
    Q_i_init.store_in(&buf_Q_i);
    O_r_init.store_in(&buf_O_r);
    O_i_init.store_in(&buf_O_i);
    P_r_init.store_in(&buf_P_r);
    P_i_init.store_in(&buf_P_i);

    Q_r_update.store_in(&buf_Q_r, {n, iCprime, iSprime, kCprime, kSprime, jCprime, jSprime, x, t, y});
    Q_i_update.store_in(&buf_Q_i, {n, iCprime, iSprime, kCprime, kSprime, jCprime, jSprime, x, t, y});
    O_r_update.store_in(&buf_O_r, {n, jCprime, jSprime, kCprime, kSprime, iCprime, iSprime, x, t, y});
    O_i_update.store_in(&buf_O_i, {n, jCprime, jSprime, kCprime, kSprime, iCprime, iSprime, x, t, y});
    P_r_update.store_in(&buf_P_r, {n, jCprime, jSprime, kCprime, kSprime, iCprime, iSprime, x, t, y});
    P_i_update.store_in(&buf_P_i, {n, jCprime, jSprime, kCprime, kSprime, iCprime, iSprime, x, t, y});

    Bsingle_r_init.store_in(&buf_Bsingle_r);
    Bsingle_i_init.store_in(&buf_Bsingle_i);

    Bsingle_r_update.store_in(&buf_Bsingle_r, {n, iCprime, iSprime, jCprime, jSprime, kCprime, kSprime, x, x2, t});
    Bsingle_i_update.store_in(&buf_Bsingle_i, {n, iCprime, iSprime, jCprime, jSprime, kCprime, kSprime, x, x2, t});

    Bdouble_r_init.store_in(&buf_Bdouble_r);
    Bdouble_i_init.store_in(&buf_Bdouble_i);
    Bdouble_r_update0.store_in(&buf_Bdouble_r, {n, iCprime, iSprime, jCprime, jSprime, kCprime, kSprime, x, x2, t});
    Bdouble_i_update0.store_in(&buf_Bdouble_i, {n, iCprime, iSprime, jCprime, jSprime, kCprime, kSprime, x, x2, t});
    Bdouble_r_update1.store_in(&buf_Bdouble_r, {n, iCprime, iSprime, jCprime, jSprime, kCprime, kSprime, x, x2, t});
    Bdouble_i_update1.store_in(&buf_Bdouble_i, {n, iCprime, iSprime, jCprime, jSprime, kCprime, kSprime, x, x2, t});

    // -------------------------------------------------------
    // Code Generation
    // -------------------------------------------------------
    tiramisu::codegen({&buf_Blocal_r, &buf_Blocal_i, prop_r.get_buffer(), prop_i.get_buffer(), weights.get_buffer(), psi_r.get_buffer(), psi_i.get_buffer(), color_weights.get_buffer(), spin_weights.get_buffer(), Bsingle_r_update.get_buffer(), Bsingle_i_update.get_buffer(), Bdouble_r_init.get_buffer(), Bdouble_i_init.get_buffer(), &buf_O_r, &buf_O_i, &buf_P_r, &buf_P_i, &buf_Q_r, &buf_Q_i}, "generated_baryon.o");
}

int main(int argc, char **argv)
{
    generate_function("tiramisu_generated_code");

    return 0;
}