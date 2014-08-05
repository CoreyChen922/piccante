/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

PICCANTE is free software; you can redistribute it and/or modify
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 3.0 of
the License, or (at your option) any later version.

PICCANTE is distributed in the hope that it will be useful, but
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License
( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.

*/

#ifndef PIC_NELDER_MEAD_OPT_FUNDAMENTAL_HPP
#define PIC_NELDER_MEAD_OPT_FUNDAMENTAL_HPP

#include "util/eigen_util.hpp"
#include "util/matrix_3_x_3.hpp"
#include "util/nelder_mead_opt_base.hpp"

#ifndef PIC_DISABLE_EIGEN
   #include "externals/Eigen/Dense"
#endif

namespace pic {

#ifndef PIC_DISABLE_EIGEN

class NelderMeadOptFundamental: public NelderMeadOptBase<double>
{
public:
    std::vector< pic::Vec<2, float> > m0, m1;

    /**
     * @brief NelderMeadOptFundamental
     * @param m0
     * @param m1
     * @param inliers
     */
    NelderMeadOptFundamental(std::vector< pic::Vec<2, float> > &m0,
                             std::vector< pic::Vec<2, float> > &m1,
                             std::vector< unsigned int> inliers) : NelderMeadOptBase()
    {
        if(!inliers.empty()) {
            for(unsigned int i = 0; i < inliers.size(); i++) {
                this->m0.push_back(m0[inliers[i]]);
                this->m1.push_back(m1[inliers[i]]);
            }
        } else {
            this->m0.assign(m0.begin(), m0.end());
            this->m1.assign(m1.begin(), m1.end());
        }
    }

    /**
     * @brief Fundamental
     * @param F
     * @param p
     * @return
     */
    double FundamentalDistance(Eigen::Matrix3d &F, Eigen::Vector3d &p0, Eigen::Vector3d  &p1)
    {        
        Eigen::Vector3d F_p0 = F * p0;

        double norm = F_p0[0] * F_p0[0] + F_p0[1] * F_p0[1];
        if(norm > 0.0) {
            norm = sqrt(norm);

            F_p0[0] /= norm;
            F_p0[1] /= norm;
            F_p0[2] /= norm;
        }

        //computing distance

        return F_p0.dot(p1);
    }

    /**
     * @brief function
     * @param x
     * @param n
     * @return
     */
    float function(float *x, unsigned int n)
    {
        Eigen::Matrix3d F = getMatrixFromLinearArray(x, 3, 3);
        Eigen::Matrix3d F_t = Eigen::Transpose<Eigen::Matrix3d>(F);

        double err = 0.0;
        for(unsigned int i = 0; i < m0.size(); i++) {

            Eigen::Vector3d p0 = Eigen::Vector3d(m0[i][0], m0[i][1], 1.0);
            Eigen::Vector3d p1 = Eigen::Vector3d(m1[i][0], m1[i][1], 1.0);

            double tmp_err;

            // | p1^t F p0 | error           
            tmp_err = FundamentalDistance(F, p0, p1);
            err += tmp_err * tmp_err;

            // | p0^t F^t p1 | error
            tmp_err = FundamentalDistance(F_t, p1, p0);
            err += tmp_err * tmp_err;
        }

        return float(err);
    }
};
#endif

}

#endif // PIC_NELDER_MEAD_OPT_FUNDAMENTAL_HPP
