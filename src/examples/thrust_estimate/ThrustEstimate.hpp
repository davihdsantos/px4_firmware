/****************************************************************************
 *
 *   Copyright (c) 2020-2021 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#pragma once

#include <px4_platform_common/defines.h>
#include <px4_platform_common/module.h>
#include <px4_platform_common/module_params.h>
#include <px4_platform_common/posix.h>
#include <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>

#include <drivers/drv_hrt.h>
#include <lib/perf/perf_counter.h>

#include <uORB/Publication.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/SubscriptionCallback.hpp>

#include <uORB/topics/vehicle_thrust_estimate.h>
#include <uORB/topics/esc_status.h>
#include <uORB/topics/vehicle_odometry.h>

using namespace time_literals;

class currentData : public ListNode<currentData *>
{
public:
	double data{0};
};

class ThrustEstimate : public ModuleBase<ThrustEstimate>, public ModuleParams, public px4::ScheduledWorkItem
{
public:
	ThrustEstimate();
	~ThrustEstimate() override;

	/** @see ModuleBase */
	static int task_spawn(int argc, char *argv[]);

	/** @see ModuleBase */
	static int custom_command(int argc, char *argv[]);

	/** @see ModuleBase */
	static int print_usage(const char *reason = nullptr);

	bool init();

	int print_status() override;

	double compute_lambda_i(double lambda_s);

	double compute_C_T(double lambda_i, double lambda_s, double mu);

	double compute_kappa(double C_T);

	double compute_C_P_am_hat(double lambda_i, double lambda_s, double C_T, double kappa);

	double thrust_computation(double _i_hat, double _w, double _w_dot_hat, int index);

	void initialize_parameters(void);

	double updateMeanFilter(double currentMean, double newDataPoint, int windowSize, List<currentData *>& buffer);

	double compute_azero(double lambda_i, double lambda_s, double _mu);

	double compute_aum(double lambda_i, double lambda_s, double _mu);

	double compute_bum(double _mu, double azero);

	double compute_C_H(double lambda_i, double lambda_s, double _mu, double _azero, double _bum);

	double compute_mu(double vx, double vy, double _w);

	double compute_gamma();

private:
	void Run() override;

	// Publications
	uORB::Publication<vehicle_thrust_estimate_s> _thrust_estimate_pub{ORB_ID(vehicle_thrust_estimate)};

	// Subscriptions
	uORB::SubscriptionCallbackWorkItem _esc_status_sub{this, ORB_ID(esc_status)};
	uORB::SubscriptionCallbackWorkItem _vehicle_odom_sub{this, ORB_ID(vehicle_odometry)};

	// Performance (perf) counters
	perf_counter_t	_loop_perf{perf_alloc(PC_ELAPSED, MODULE_NAME": cycle")};
	perf_counter_t	_loop_interval_perf{perf_alloc(PC_INTERVAL, MODULE_NAME": interval")};

	double c[5];
	double d[2];
	double K_q[2];
	double mass;
	double radius;
	double rho;
	double thrust_scale;

	double I_r;

	double Delta;
	int N, N_b;
	double epsilon;

	double _vx, _vy;

	double old_lambda_s_k[4];

	double w[4];
	double w_old[4];
	double w_dot_hat[4];
	hrt_abstime begin[4];
	// hrt_abstime now[4];
	clock_t begin_all;
	clock_t end_all;
	double thrust[4];
	double old_thrust[4];
	double i_hat[4];

	uint32_t _sensor_interval_us{1250};

	List<currentData *> _filterListCurrent[4];
	double _currentMean[4];

	List<currentData *> _filterListRpm[4];
	double _rpmMean[4];
	double _rpmMean_old[4];
	double _rpmMean_dot[4];
	int _windowSize = 10;

	hrt_abstime _begin_all;

};
