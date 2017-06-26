////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ProtoNNTrainerUtils.h (trainers)
//  Authors:  Suresh Iyengar
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// data
#include "Dataset.h"
#include "Example.h"

// stl
#include <cstddef>
#include <memory>

// Matrix
#include <Matrix.h>

namespace ell
{
namespace trainers
{
    class ProtoNNTrainerUtils
    {
    public:
        static void ProtoNNTrainerUtils::GetDatasetAsMatrix(const data::AutoSupervisedDataset& anyDataset, math::MatrixReference<double, math::MatrixLayout::columnMajor> X, math::MatrixReference<double, math::MatrixLayout::columnMajor> Y);

        template <typename math::MatrixLayout Layout>
        static math::Matrix<double, Layout> MatrixExp(math::ConstMatrixReference<double, Layout> A);

        template <typename math::MatrixLayout Layout>
        static double MatrixNorm(math::ConstMatrixReference<double, Layout> A);

        template <typename math::MatrixLayout Layout>
        static double MaxAbsoluteElement(math::ConstMatrixReference<double, Layout> A);

        static void HardThresholding(math::MatrixReference<double, math::MatrixLayout::columnMajor> M, double sparsity);
    };
}
}

#include "../tcc/ProtoNNTrainerUtils.tcc"