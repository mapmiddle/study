extern "C" {
    #include "onnxruntime.h"
    #include <stdio.h>
    #include <assert.h>

    const OrtApi* g_ort = getApi();

    const OrtApi* getApi() {
        return OrtGetApiBase()->GetApi(ORT_API_VERSION);
    }

    void check(OrtStatus* status) {
        if (status != NULL) {
            const char* msg = g_ort->GetErrorMessage(status);
            fprintf(stderr, "%s\n", msg);
            g_ort->ReleaseStatus(status);
            exit(1);
        }
    }

    void onnxTest() {
        printf("- step1\n");
        OrtEnv* env;
        check(g_ort->CreateEnv(ORT_LOGGING_LEVEL_WARNING, "test", &env));

        printf("- step2\n");
        // initialize session options if needed
        OrtSessionOptions* session_options;
        check(g_ort->CreateSessionOptions(&session_options));
        g_ort->SetIntraOpNumThreads(session_options, 1);

        printf("- step3\n");
        // Sets graph optimization level
        g_ort->SetSessionGraphOptimizationLevel(session_options, ORT_ENABLE_BASIC);

        // create session and load model into memory
        printf("- step4\n");
        OrtSession* session;
        const char* model_path = "sample/mul_1.onnx";
        check(g_ort->CreateSession(env, model_path, session_options, &session));

        // print model input layer (node names, types, shape etc.)
        printf("- step5\n");
        size_t num_input_nodes = 0;
        size_t num_output_nodes = 0;
        OrtAllocator* allocator;
        check(g_ort->GetAllocatorWithDefaultOptions(&allocator));
        check(g_ort->SessionGetInputCount(session, &num_input_nodes));
        check(g_ort->SessionGetOutputCount(session, &num_output_nodes));
        printf("num input nodes: %d\n", num_input_nodes);
        printf("num output nodes: %d\n", num_output_nodes);

        // iterate over all input nodes
        printf("- step6-1\n");
        const char** input_node_names = new const char*[num_input_nodes];
        int64_t* input_node_dims = NULL;
        size_t input_node_dims_len = 1;
        size_t input_tensor_len = 1;

        for (size_t i = 0; i < num_input_nodes; i++) {
            // print input node names
            char* input_name;
            check(g_ort->SessionGetInputName(session, i, allocator, &input_name));
            printf("Input %zu : name=%s\n", i, input_name);
            input_node_names[i] = input_name;

            // print input node types
            OrtTypeInfo* typeinfo;
            check(g_ort->SessionGetInputTypeInfo(session, i, &typeinfo));
            const OrtTensorTypeAndShapeInfo* tensor_info;
            check(g_ort->CastTypeInfoToTensorInfo(typeinfo, &tensor_info));
            ONNXTensorElementDataType type;
            check(g_ort->GetTensorElementType(tensor_info, &type));
            printf("Input %zu : type=%d\n", i, type);

            // print input shapes/dims
            check(g_ort->GetDimensionsCount(tensor_info, &input_node_dims_len));
            printf("Input %zu : input_node_dims_len=%zu\n", i, input_node_dims_len);

            if (input_node_dims != NULL) {
                delete input_node_dims;
            }
            input_node_dims = new int64_t[input_node_dims_len];

            g_ort->GetDimensions(tensor_info, (int64_t*)input_node_dims, input_node_dims_len);
            for (size_t j = 0; j < input_node_dims_len; j++) {
                
                printf("Input %zu : dim %zu=%jd\n", i, j, input_node_dims[j]);
                input_tensor_len *= static_cast<size_t>(input_node_dims[j]);
            }

            g_ort->ReleaseTypeInfo(typeinfo);
        }

        // iterate over all output nodes
        printf("- step6-2\n");
        const char** output_node_names = new const char*[num_output_nodes];
        int64_t* output_node_dims = NULL;
        size_t output_node_dims_len = 1;
        size_t output_tensor_len = 1;

        for (size_t i = 0; i < num_output_nodes; i++) {
            // print output node names
            char* output_name;
            check(g_ort->SessionGetOutputName(session, i, allocator, &output_name));

            printf("Output %zu : name=%s\n", i, output_name);
            output_node_names[i] = output_name;

            // print output node types
            OrtTypeInfo* typeinfo;
            check(g_ort->SessionGetOutputTypeInfo(session, i, &typeinfo));
            const OrtTensorTypeAndShapeInfo* tensor_info;
            check(g_ort->CastTypeInfoToTensorInfo(typeinfo, &tensor_info));
            ONNXTensorElementDataType type;
            check(g_ort->GetTensorElementType(tensor_info, &type));
            printf("Output %zu : type=%d\n", i, type);

            // print output shapes/dims
            check(g_ort->GetDimensionsCount(tensor_info, &output_node_dims_len));
            printf("Output %zu : output_node_dims_len=%zu\n", i, output_node_dims_len);

            if (output_node_dims != NULL) {
                delete output_node_dims;
            }
            output_node_dims = new int64_t[output_node_dims_len];

            g_ort->GetDimensions(tensor_info, (int64_t*)output_node_dims, output_node_dims_len);
            for (size_t j = 0; j < output_node_dims_len; j++) {
                printf("Output %zu : dim %zu=%jd\n", i, j, output_node_dims[j]);
                output_tensor_len *= static_cast<size_t>(output_node_dims[j]);
            }

            g_ort->ReleaseTypeInfo(typeinfo);
        }

        // create input tensor object from data values
        printf("- step7\n");
        float input_tensor_values[input_tensor_len];
        for (size_t i = 0; i < input_tensor_len; i++) {
            input_tensor_values[i] = (float)2.0;
        }

        OrtMemoryInfo* memory_info;
        check(g_ort->CreateCpuMemoryInfo(OrtArenaAllocator, OrtMemTypeDefault, &memory_info));

        const size_t input_tensor_size = input_tensor_len * sizeof(float);
        printf("tensor count : %d\n", input_tensor_len);
        printf("tensor size : %d\n", input_tensor_size);
        OrtValue* input_tensor = NULL;
        
        check(g_ort->CreateTensorWithDataAsOrtValue(memory_info, 
            input_tensor_values, input_tensor_size, 
            input_node_dims, input_node_dims_len, 
            ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT, 
            &input_tensor));

        int is_tensor;
        check(g_ort->IsTensor(input_tensor, &is_tensor));
        assert(is_tensor);
        g_ort->ReleaseMemoryInfo(memory_info);

        printf("- step8\n");
        OrtValue* output_tensor = NULL;
        check(g_ort->Run(session, NULL, 
            input_node_names, (const OrtValue* const*)&input_tensor, num_input_nodes, 
            output_node_names, num_output_nodes, &output_tensor));
        check(g_ort->IsTensor(output_tensor, &is_tensor));
        assert(is_tensor);
        
        printf("- step9\n");
        float* result;
        check(g_ort->GetTensorMutableData(output_tensor, (void**)&result));

        for (int i = 0; i <6; i++)
            printf("Score for class [%d] =  %f\n", i, result[i]);

        g_ort->ReleaseValue(output_tensor);
        g_ort->ReleaseValue(input_tensor);
        g_ort->ReleaseSession(session);
        g_ort->ReleaseSessionOptions(session_options);
        g_ort->ReleaseEnv(env);
        
        fflush(stdout);
        // g_ort->ReleaseEnv(env);
    }
}