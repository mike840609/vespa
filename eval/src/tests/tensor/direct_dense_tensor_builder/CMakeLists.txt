# Copyright 2017 Yahoo Holdings. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.
vespa_add_executable(eval_direct_dense_tensor_builder_test_app TEST
    SOURCES
    direct_dense_tensor_builder_test.cpp
    DEPENDS
    vespaeval
)
vespa_add_test(NAME eval_direct_dense_tensor_builder_test_app COMMAND eval_direct_dense_tensor_builder_test_app)
