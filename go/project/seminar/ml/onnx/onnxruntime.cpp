extern "C" {
    #include "onnxruntime.h"
    #include <stdio.h>

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
        const char* model_path = "sample/model.onnx";
        check(g_ort->CreateSession(env, model_path, session_options, &session));

        // print model input layer (node names, types, shape etc.)
        printf("- step5\n");
        size_t num_input_nodes = 0;
        OrtAllocator* allocator;
        check(g_ort->GetAllocatorWithDefaultOptions(&allocator));
        check(g_ort->SessionGetInputCount(session, &num_input_nodes));
        printf("num input nodes: %d\n", num_input_nodes);

        // iterate over all input nodes
        const char** input_node_names = new const char*[num_input_nodes];
        int64_t* input_node_dims = NULL;

        printf("- step6\n");
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
            size_t num_dims;
            check(g_ort->GetDimensionsCount(tensor_info, &num_dims));
            printf("Input %zu : num_dims=%zu\n", i, num_dims);

            if (input_node_dims != NULL) {
                delete input_node_dims;
            }
            input_node_dims = new int64_t[num_dims];

            g_ort->GetDimensions(tensor_info, (int64_t*)input_node_dims, num_dims);
            for (size_t j = 0; j < num_dims; j++)
            printf("Input %zu : dim %zu=%jd\n", i, j, input_node_dims[j]);

            g_ort->ReleaseTypeInfo(typeinfo);
        }

        // create input tensor object from data values
        printf("- step7\n");
        const size_t input_tensor_size = 224 * 224 * 3;  
        float input_tensor_values[input_tensor_size];
        for (size_t i = 0; i < input_tensor_size; i++) {
            input_tensor_values[i] = (float)i / (input_tensor_size + 1);
        }

        OrtMemoryInfo* memory_info;
        check(g_ort->CreateCpuMemoryInfo(OrtArenaAllocator, OrtMemTypeDefault, &memory_info));

        OrtValue* input_tensor = NULL;
        check(g_ort->CreateTensorWithDataAsOrtValue(memory_info, 
            input_tensor_values, 
            input_tensor_size * sizeof(float), 
            input_node_dims, 
            4, ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT, 
            &input_tensor));

        int is_tensor;
        check(g_ort->IsTensor(input_tensor, &is_tensor));

        fflush(stdout);
        // g_ort->ReleaseEnv(env);
    }
}