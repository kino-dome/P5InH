
#ifndef P5_IN_HOUDINI
#define P5_IN_HOUDINI


// HELPER FUNCTIONS Position Direction Scaler

vector fromPixelSpaceP(vector aPixelPosition; string aCameraPath)
{
    float cameraNear = chf(aCameraPath + "/near");
    float cameraFar = chf(aCameraPath + "/far");
    vector2 cameraRes = set(chf(aCameraPath + "/resx"), chf(aCameraPath + "/resy"));
        
    vector bottomLeftHalfDepth = fromNDC(aCameraPath, set(0,0, -(cameraFar-cameraNear)/2.0));
    vector bottomRightHalfDepth = fromNDC(aCameraPath, set(1,0, -(cameraFar-cameraNear)/2.0));

    // if I'm right this is the size of a pixel in H metrics
    float coef = distance(bottomLeftHalfDepth, bottomRightHalfDepth) / cameraRes.x;
    float maxDim = ((cameraFar - cameraNear) / coef) / 2.0;

    vector ndcPos;
    ndcPos.z = efit(aPixelPosition.z, -maxDim, maxDim, -cameraFar, -cameraNear);
    ndcPos.x = efit(aPixelPosition.x, 0, cameraRes.x, 0, 1);
    ndcPos.y = efit(aPixelPosition.y, cameraRes.y, 0, 0, 1);

    return fromNDC(aCameraPath, ndcPos);
}

vector fromPixelSpaceD(vector aDirection; string aCameraPath)
{
    vector pos1 = fromPixelSpaceP(set(0.0, 0.0, 0.0), aCameraPath);
    vector pos2 = fromPixelSpaceP(aDirection, aCameraPath);
    return pos2 - pos1;
}

float fromPixelSpaceS(float aScaler; string aCameraPath)
{
    vector2 cameraRes = set(chf(aCameraPath + "/resx"), chf(aCameraPath + "/resy"));
    float cameraNear = chf(aCameraPath + "/near");
    float cameraFar = chf(aCameraPath + "/far");

    vector bottomLeftHalfDepth = fromNDC(aCameraPath, set(0,0, -(cameraFar-cameraNear)/2.0));
    vector bottomRightHalfDepth = fromNDC(aCameraPath, set(1,0, -(cameraFar-cameraNear)/2.0));

    // if I'm right this is the size of a pixel in H metrics
    float coef = distance(bottomLeftHalfDepth, bottomRightHalfDepth) / cameraRes.x;
    return aScaler * coef;
}

vector toPixelSpaceP(vector aWorldPosition; string aCameraPath)
{
    float cameraNear = chf(aCameraPath + "/near");
    float cameraFar = chf(aCameraPath + "/far");
    vector2 cameraRes = set(chf(aCameraPath + "/resx"), chf(aCameraPath + "/resy"));
    vector pixelPos;

    vector bottomLeftHalfDepth = fromNDC(aCameraPath, set(0,0, -(cameraFar-cameraNear)/2.0));
    vector bottomRightHalfDepth = fromNDC(aCameraPath, set(1,0, -(cameraFar-cameraNear)/2.0));
    float coef = distance(bottomLeftHalfDepth, bottomRightHalfDepth) / cameraRes.x;
    float maxDim = ((cameraFar - cameraNear) / coef) / 2.0;

    vector ndcPos = toNDC(aCameraPath, aWorldPosition);
    pixelPos.x = efit(ndcPos.x, 0, 1, 0, cameraRes.x);
    pixelPos.y = efit(ndcPos.y, 0, 1, cameraRes.y, 0);
    pixelPos.z = efit(ndcPos.z, -cameraFar, -cameraNear, -maxDim, maxDim);

    return pixelPos;
}

vector toPixelSpaceD(vector aDirection; string aCameraPath)
{
    vector pos1 = toPixelSpaceP(set(0.0, 0.0, 0.0), aCameraPath);
    vector pos2 = toPixelSpaceP(aDirection, aCameraPath);
    return pos2 - pos1;
}

float toPixelSpaceS(float aScaler; string aCameraPath)
{
    vector2 cameraRes = set(chf(aCameraPath + "/resx"), chf(aCameraPath + "/resy"));
    float cameraNear = chf(aCameraPath + "/near");
    float cameraFar = chf(aCameraPath + "/far");

    vector bottomLeftHalfDepth = fromNDC(aCameraPath, set(0,0, -(cameraFar-cameraNear)/2.0));
    vector bottomRightHalfDepth = fromNDC(aCameraPath, set(1,0, -(cameraFar-cameraNear)/2.0));

    // if I'm right this is the size of a pixel in H metrics
    float coef = distance(bottomLeftHalfDepth, bottomRightHalfDepth) / cameraRes.x;
    return aScaler / coef;
}

struct P5
{
    int         mIsP5Setup;
    float       mStrokeWeight;
    int         mHasFill;
    int         mHasStroke;
    vector4     mFillColor;
    vector4     mStrokeColor;
    vector4     mBackgroundColor;
    matrix      mMatrixList[];
    int         mShouldClearBuffer;
    //these three are the same as matrix
    vector      mTransList[];
    vector      mRotList[];
    vector      mScaleList[];
    //modes
    string      mStrokeJoin;
    string      mRectMode;
    string      mEllipseMode;
    vector2     mSphereDetail;    
    // should use pixel space
    int         mShouldUsePixelSpace;
    string      mCameraPath;
    //Envoirment variables
    int         width;
    int         height;

    void open()
    {
        mIsP5Setup = attrib(geoself(), "detail", "P5_mIsP5Setup", 0);
        mStrokeWeight = attrib(geoself(), "detail", "P5_mStrokeWeight", 0);
        mHasFill = attrib(geoself(), "detail", "P5_mHasFill", 0);
        mHasStroke = attrib(geoself(), "detail", "P5_mHasStroke", 0);
        mFillColor = attrib(geoself(), "detail", "P5_mFillColor", 0);
        mStrokeColor = attrib(geoself(), "detail", "P5_mStrokeColor", 0);
        mBackgroundColor = attrib(geoself(), "detail", "P5_mBackgroundColor", 0);
        mMatrixList = attrib(geoself(), "detail", "P5_mMatrixList", 0);
        mTransList = attrib(geoself(), "detail", "P5_mTransList", 0);
        mRotList = attrib(geoself(), "detail", "P5_mRotList", 0);
        mScaleList = attrib(geoself(), "detail", "P5_mScaleList", 0);
        mStrokeJoin = attrib(geoself(), "detail", "P5_mStrokeJoin", 0);
        mRectMode = attrib(geoself(), "detail", "P5_mRectMode", 0);
        mEllipseMode = attrib(geoself(), "detail", "P5_mEllipseMode", 0);
        mSphereDetail = attrib(geoself(), "detail", "P5_mSphereDetail", 0);
        mShouldUsePixelSpace = attrib(geoself(), "detail", "P5_mShouldUsePixelSpace", 0);
        mCameraPath = attrib(geoself(), "detail", "P5_mCameraPath", 0);
        mShouldClearBuffer = attrib(geoself(), "detail", "P5_mShouldClearBuffer", 0);

        //at the begining of each loop the matrix is reset
        mMatrixList[len(mMatrixList)-1] = ident();
        mTransList[len(mTransList)-1] = set(0.0, 0.0, 0.0);
        mRotList[len(mRotList)-1] = set(0.0, 0.0, 0.0);
        mScaleList[len(mScaleList)-1] = set(1.0, 1.0, 1.0);

        //envoirment variables
        if (mCameraPath != "NONE"){
            width = chi(mCameraPath+"/resx");
            height = chi(mCameraPath+"/resy");
        }
    }

    void close()
    {
        setattrib(geoself(), "detail", "P5_mIsP5Setup", 0, 0, mIsP5Setup, "set");
        setattrib(geoself(), "detail", "P5_mStrokeWeight", 0, 0, mStrokeWeight, "set");
        setattrib(geoself(), "detail", "P5_mHasFill", 0, 0, mHasFill, "set");
        setattrib(geoself(), "detail", "P5_mHasStroke", 0, 0, mHasStroke, "set");
        setattrib(geoself(), "detail", "P5_mFillColor", 0, 0, mFillColor, "set");
        setattrib(geoself(), "detail", "P5_mStrokeColor", 0, 0, mStrokeColor, "set");
        setattrib(geoself(), "detail", "P5_mBackgroundColor", 0, 0, mBackgroundColor, "set");
        setattrib(geoself(), "detail", "P5_mMatrixList", 0, 0, mMatrixList, "set");
        setattrib(geoself(), "detail", "P5_mTransList", 0, 0, mTransList, "set");
        setattrib(geoself(), "detail", "P5_mRotList", 0, 0, mRotList, "set");
        setattrib(geoself(), "detail", "P5_mScaleList", 0, 0, mScaleList, "set");
        setattrib(geoself(), "detail", "P5_mStrokeJoin", 0, 0, mStrokeJoin, "set");
        setattrib(geoself(), "detail", "P5_mRectMode", 0, 0, mRectMode, "set");
        setattrib(geoself(), "detail", "P5_mEllipseMode", 0, 0, mEllipseMode, "set");
        setattrib(geoself(), "detail", "P5_mSphereDetail", 0, 0, mSphereDetail, "set");
        setattrib(geoself(), "detail", "P5_mShouldUsePixelSpace", 0, 0, mShouldUsePixelSpace, "set");
        setattrib(geoself(), "detail", "P5_mCameraPath", 0, 0, mCameraPath, "set");
        setattrib(geoself(), "detail", "P5_mShouldClearBuffer", 0, 0, mShouldClearBuffer, "set");
    }

    void setup()
    {
        int s;

        mIsP5Setup = 1;
        mStrokeWeight = 1.0;
        mStrokeColor = set(0.0, 0.0, 0.0, 1.0);
        mFillColor = set(1.0, 1.0, 1.0, 1.0);
        mHasStroke = 1;
        mHasFill = 1;
        mBackgroundColor = set(0.0, 0.0, 0.0, 0.0);
        push(mMatrixList, ident());
        push(mTransList, set(0.0, 0.0, 0.0));
        push(mRotList, set(0.0, 0.0, 0.0));
        push(mScaleList, set(1.0, 1.0, 1.0));
        mStrokeJoin = "ROUND";
        mEllipseMode = "CENTER";
        mRectMode = "CORNER";
        mSphereDetail = set(12, 24);
        mShouldUsePixelSpace = 0;
        mCameraPath = "";
        mShouldClearBuffer = 0;

        if (attribtypeinfo(geoself(), "detail", "P5_mIsP5Setup") == ""){
            //it's our first time, need to make the attribs
            // detail attribs for the main P5 instance
            addattrib(geoself(), "detail", "P5_mIsP5Setup", mIsP5Setup);
            addattrib(geoself(), "detail", "P5_mStrokeWeight", mStrokeWeight);
            addattrib(geoself(), "detail", "P5_mHasFill", mHasFill);
            addattrib(geoself(), "detail", "P5_mHasStroke", mHasStroke);
            addattrib(geoself(), "detail", "P5_mFillColor", mFillColor);
            addattrib(geoself(), "detail", "P5_mStrokeColor", mStrokeColor);
            addattrib(geoself(), "detail", "P5_mBackgroundColor", mBackgroundColor);
            addattrib(geoself(), "detail", "P5_mMatrixList", {});
            addattrib(geoself(), "detail", "P5_mTransList", {});
            addattrib(geoself(), "detail", "P5_mRotList", {});
            addattrib(geoself(), "detail", "P5_mScaleList", {});
            addattrib(geoself(), "detail", "P5_mStrokeJoin", mStrokeJoin);
            addattrib(geoself(), "detail", "P5_mEllipseMode", mEllipseMode);
            addattrib(geoself(), "detail", "P5_mRectMode", mRectMode);
            addattrib(geoself(), "detail", "P5_mSphereDetail", mSphereDetail);
            addattrib(geoself(), "detail", "P5_mShouldUsePixelSpace", mShouldUsePixelSpace);
            addattrib(geoself(), "detail", "P5_mCameraPath", mCameraPath);
            addattrib(geoself(), "detail", "P5_mShouldClearBuffer", mShouldClearBuffer);

            // point attribs needed for the P5 maker
            addattrib(geoself(), "point", "P5_mStrokeWeight", mStrokeWeight);
            addattrib(geoself(), "point", "P5_mHasFill", mHasFill);
            addattrib(geoself(), "point", "P5_mHasStroke", mHasStroke);
            addattrib(geoself(), "point", "P5_mFillColor", mFillColor);
            addattrib(geoself(), "point", "P5_mStrokeColor", mStrokeColor);
            addattrib(geoself(), "point", "P5_mMatrix", matrix(ident()));
            addattrib(geoself(), "point", "P5_mTrans", set(0.0, 0.0, 0.0));
            addattrib(geoself(), "point", "P5_mRot", set(0.0, 0.0, 0.0));
            addattrib(geoself(), "point", "P5_mScale", set(1.0, 1.0, 1.0));
            addattrib(geoself(), "point", "P5_mGeoType", "");
            addattrib(geoself(), "point", "P5_mGeoParams", {}); // this is for passing params for each geo type
            addattrib(geoself(), "point", "P5_mStrokeJoin", mStrokeJoin);
            addattrib(geoself(), "point", "P5_mEllipseMode", mEllipseMode);
            addattrib(geoself(), "point", "P5_mRectMode", mRectMode);
        }

        this -> close();
    }

    // PixelSpace Transformation
    vector fromPixelSpaceP(vector aPixelPosition)
    {
        return fromPixelSpaceP(aPixelPosition, mCameraPath);
    }

    vector fromPixelSpaceD(vector aPixelDirection)
    {
        return fromPixelSpaceD(aPixelDirection, mCameraPath);
    }

    float fromPixelSpaceS(float aPixelScaler)
    {
        return fromPixelSpaceS(aPixelScaler, mCameraPath);
    }

    vector toPixelSpaceP(vector aWorldPosition)
    {
        return toPixelSpaceP(aWorldPosition, mCameraPath);
    }

    vector toPixelSpaceD(vector aWorldDirection)
    {
        return toPixelSpaceD(aWorldDirection, mCameraPath);
    }

    float toPixelSpaceS(float aWorldScaler)
    {
        return toPixelSpaceS(aWorldScaler, mCameraPath);
    }


    // HELPER METHODS

    matrix getCurrentMat()
    {
        return mMatrixList[len(mMatrixList) - 1];
    }

    vector getCurrentTrans()
    {
        return mTransList[len(mTransList) - 1];
    }

    vector getCurrentRot()
    {
        return mRotList[len(mRotList) - 1];
    }

    vector getCurrentScale()
    {
        return mScaleList[len(mScaleList) - 1];
    }

    void setCurrentMat(matrix aMat)
    {
        mMatrixList[len(mMatrixList) - 1] = aMat;
    }

    void setCurrentTrans(vector aTrans)
    {
        mTransList[len(mTransList)-1] = aTrans;
    }

    void setCurrentRot(vector aRot)
    {
        mRotList[len(mRotList)-1] = aRot;
    }

    void setCurrentScale(vector aScale)
    {
        mScaleList[len(mScaleList)-1] = aScale;
    }

    void transferAttribsToPoint(int aPointNum)
    {   
        setattrib(geoself(), "point", "P5_mStrokeWeight", aPointNum, 0, mStrokeWeight, "set");
        setattrib(geoself(), "point", "P5_mHasFill", aPointNum, 0, mHasFill, "set");
        setattrib(geoself(), "point", "P5_mHasStroke", aPointNum, 0, mHasStroke, "set");
        setattrib(geoself(), "point", "P5_mFillColor", aPointNum, 0, mFillColor, "set");
        setattrib(geoself(), "point", "P5_mStrokeColor", aPointNum, 0, mStrokeColor, "set");
        setattrib(geoself(), "point", "P5_mMatrix", aPointNum, 0, this -> getCurrentMat(), "set");
        setattrib(geoself(), "point", "P5_mTrans", aPointNum, 0, this -> getCurrentTrans(), "set");
        setattrib(geoself(), "point", "P5_mRot", aPointNum, 0, this -> getCurrentRot(), "set");
        setattrib(geoself(), "point", "P5_mScale", aPointNum, 0, this -> getCurrentScale(), "set");
        setattrib(geoself(), "point", "P5_mStrokeJoin", aPointNum, 0, mStrokeJoin, "set");
        setattrib(geoself(), "point", "P5_mEllipseMode", aPointNum, 0, mEllipseMode, "set");
        setattrib(geoself(), "point", "P5_mRectMode", aPointNum, 0, mRectMode, "set");
    }

    

    void setCameraPath(string aCameraPath)
    {
        mCameraPath = aCameraPath;
    }

    // DRAWING
    

    void ellipse (float a,b,c,d)
    {
        int pointNum = addpoint(geoself(), set(0.0, 0.0, 0.0));
        setpointgroup(geoself(), "P5_InstancePoints", pointNum, 1, "set");
        float params[]; 
        if (mShouldUsePixelSpace){
            vector pixelPos = set(a, b, 0);
            vector worldPos = fromPixelSpaceP(pixelPos, mCameraPath);
            if (mEllipseMode == "CENTER" || mEllipseMode == "RADIUS"){
                push(params, worldPos.x); push(params, worldPos.y);
                push(params, fromPixelSpaceS(c, mCameraPath)); push(params, fromPixelSpaceS(d, mCameraPath));
            }
            else{ // CORNER || CORNERS
                push(params, worldPos.x); push(params, worldPos.y);
                worldPos = fromPixelSpaceP(set(c, d, 0));
                push(params, worldPos.x); push(params, worldPos.y);
            }
            
        }
        else{
            push(params, a); push(params, b); push(params, c); push(params, d);
        }
        
        setattrib(geoself(), "point", "P5_mGeoType", pointNum, 0, "Ellipse", "set");
        setattrib(geoself(), "point", "P5_mGeoParams", pointNum, 0, params, "set");
        this -> transferAttribsToPoint(pointNum);
    }

    void rect (float a,b,c,d)
    {
        int pointNum = addpoint(geoself(), set(0.0, 0.0, 0.0));
        setpointgroup(geoself(), "P5_InstancePoints", pointNum, 1, "set");
        float params[];
        if (mShouldUsePixelSpace){
            vector pixelPos = set(a, b, 0);
            vector worldPos = fromPixelSpaceP(pixelPos, mCameraPath);
            if (mRecteMode == "CENTER" || mRectMode == "RADIUS"){
                push(params, worldPos.x); push(params, worldPos.y);
                push(params, fromPixelSpaceS(c, mCameraPath)); push(params, fromPixelSpaceS(d, mCameraPath));
            }
            else{ // CORNER || CORNERS
                push(params, worldPos.x); push(params, worldPos.y);
                worldPos = fromPixelSpaceP(set(c, d, 0));
                push(params, worldPos.x); push(params, worldPos.y);
            }
        }
        else{
            push(params, a); push(params, b); push(params, c); push(params, d);
        }
        setattrib(geoself(), "point", "P5_mGeoType", pointNum, 0, "Rect", "set");
        setattrib(geoself(), "point", "P5_mGeoParams", pointNum, 0, params, "set");
        this -> transferAttribsToPoint(pointNum);
    }

    void arc (float a,b,c,d, aStartAngle, aEndAngle; string aMode)
    {
        int pointNum = addpoint(geoself(), set(0.0, 0.0, 0.0));
        setpointgroup(geoself(), "P5_InstancePoints", pointNum, 1, "set");
        int mode;
        if (aMode == "OPEN") mode = 0;
        else if (aMode == "CHORD") mode = 1;
        else if (aMode == "PIE") mode = 2; 
        float params[]; 
        if (mShouldUsePixelSpace){
            vector pixelPos = set(a, b, 0);
            vector worldPos = fromPixelSpaceP(pixelPos, mCameraPath);
            if (mEllipseMode == "CENTER" || mEllipseMode == "RADIUS"){
                push(params, worldPos.x); push(params, worldPos.y);
                push(params, fromPixelSpaceS(c, mCameraPath)); push(params, fromPixelSpaceS(d, mCameraPath));
            }
            else{ // CORNER || CORNERS
                push(params, worldPos.x); push(params, worldPos.y);
                worldPos = fromPixelSpaceP(set(c, d, 0));
                push(params, worldPos.x); push(params, worldPos.y);
            }
        }
        else{
            push(params, a); push(params, b); push(params, c); push(params, d);
        }
        push(params, aStartAngle); push(params, aEndAngle); push(params, mode);
        setattrib(geoself(), "point", "P5_mGeoType", pointNum, 0, "Arc", "set");
        setattrib(geoself(), "point", "P5_mGeoParams", pointNum, 0, params, "set");
        this -> transferAttribsToPoint(pointNum);
    }

    void line(vector aStart, aEnd)
    {
        if (mHasStroke == 0) return;

        int pointNum = addpoint(geoself(), set(0.0, 0.0, 0.0));
        setpointgroup(geoself(), "P5_InstancePoints", pointNum, 1, "set");
        float params[];
        if (mShouldUsePixelSpace){
            vector worldPosStart = fromPixelSpaceP(aStart, mCameraPath);
            vector worldPosEnd = fromPixelSpaceP(aEnd, mCameraPath);
            push(params, worldPosStart.x); push(params, worldPosStart.y); push(params, worldPosStart.z);
            push(params, worldPosEnd.x); push(params, worldPosEnd.y); push(params, worldPosEnd.z);
        }
        else{
            push(params, aStart.x); push(params, aStart.y); push(params, aStart.z);
            push(params, aEnd.x); push(params, aEnd.y); push(params, aEnd.z);
        }
        
        setattrib(geoself(), "point", "P5_mGeoType", pointNum, 0, "Line", "set");
        setattrib(geoself(), "point", "P5_mGeoParams", pointNum, 0, params, "set");
        this -> transferAttribsToPoint(pointNum);
    }

    void point(vector aPos)
    {
        int pointNum = addpoint(geoself(), aPos);
        setpointgroup(geoself(), "P5_InstancePoints", pointNum, 1, "set");
        float params[];
        if (mShouldUsePixelSpace){
            vector worldPos = fromPixelSpaceP(aPos, mCameraPath);
            push(params, worldPos.x); push(params, worldPos.y); push(params, worldPos.z);                }
        else{
            push(params, aPos.x); push(params, aPos.y); push(params, aPos.z);        
        }

        setattrib(geoself(), "point", "P5_mGeoType", pointNum, 0, "Point", "set");
        setattrib(geoself(), "point", "P5_mGeoParams", pointNum, 0, params, "set");
        this -> transferAttribsToPoint(pointNum);
    }

    void sphere(float aRadius)
    {
        int pointNum = addpoint(geoself(), set(0.0, 0.0, 0.0));
        setpointgroup(geoself(), "P5_InstancePoints", pointNum, 1, "set");
        float params[];
        if (mShouldUsePixelSpace){
            push(params, fromPixelSpaceS(aRadius, mCameraPath));
        }
        else{
            push(params, aRadius);
        }
        push(params, mSphereDetail.x); // sphere ResX
        push(params, mSphereDetail.y); // sphere ResY

        setattrib(geoself(), "point", "P5_mGeoType", pointNum, 0, "Sphere", "set");
        setattrib(geoself(), "point", "P5_mGeoParams", pointNum, 0, params, "set");
        this -> transferAttribsToPoint(pointNum);
    }

    void box(float aSize)
    {
        int pointNum = addpoint(geoself(), set(0.0, 0.0, 0.0));
        setpointgroup(geoself(), "P5_InstancePoints", pointNum, 1, "set");
        float params[]; 
        if (mShouldUsePixelSpace){
            push(params, fromPixelSpaceS(aSize, mCameraPath));
        }
        else{
            push(params, aSize);
        }
        setattrib(geoself(), "point", "P5_mGeoType", pointNum, 0, "Box", "set");
        setattrib(geoself(), "point", "P5_mGeoParams", pointNum, 0, params, "set");
        this -> transferAttribsToPoint(pointNum);
    }

    void box(float aWidth, aHeight, aDepth)
    {
        int pointNum = addpoint(geoself(), set(0.0, 0.0, 0.0));
        setpointgroup(geoself(), "P5_InstancePoints", pointNum, 1, "set");
        float params[];
        if (mShouldUsePixelSpace){
            push(params, fromPixelSpaceS(aWidth, mCameraPath)); push(params, fromPixelSpaceS(aHeight, mCameraPath)); push(params, fromPixelSpaceS(aDepth, mCameraPath));
        }
        else{
            push(params, aWidth); push(params, aHeight); push(params, aDepth);
        }
        
        setattrib(geoself(), "point", "P5_mGeoType", pointNum, 0, "Box", "set");
        setattrib(geoself(), "point", "P5_mGeoParams", pointNum, 0, params, "set");
        this -> transferAttribsToPoint(pointNum);
    }

    void triangle(vector aPos1, aPos2, aPos3)
    {
        int pointNum = addpoint(geoself(), set(0.0, 0.0, 0.0));
        setpointgroup(geoself(), "P5_InstancePoints", pointNum, 1, "set");
        float params[];
        if (mShouldUsePixelSpace){
            vector worldPos1 = fromPixelSpaceP(aPos1, mCameraPath);
            vector worldPos2 = fromPixelSpaceP(aPos2, mCameraPath);
            vector worldPos3 = fromPixelSpaceP(aPos3, mCameraPath);
            push(params, worldPos1.x); push(params, worldPos1.y); push(params, worldPos1.z);
            push(params, worldPos2.x); push(params, worldPos2.y); push(params, worldPos2.z);
            push(params, worldPos3.x); push(params, worldPos3.y); push(params, worldPos3.z);
        }
        else{
            push(params, aPos1.x); push(params, aPos1.y); push(params, aPos1.z);
            push(params, aPos2.x); push(params, aPos2.y); push(params, aPos2.z);
            push(params, aPos3.x); push(params, aPos3.y); push(params, aPos3.z);
        }
        setattrib(geoself(), "point", "P5_mGeoType", pointNum, 0, "Triangle", "set");
        setattrib(geoself(), "point", "P5_mGeoParams", pointNum, 0, params, "set");
        this -> transferAttribsToPoint(pointNum);
    }

    void quad(vector aPos1, aPos2, aPos3, aPos4)
    {
        int pointNum = addpoint(geoself(), set(0.0, 0.0, 0.0));
        setpointgroup(geoself(), "P5_InstancePoints", pointNum, 1, "set");
        float params[]; 
        if (mShouldUsePixelSpace){
            vector worldPos1 = fromPixelSpaceP(aPos1, mCameraPath);
            vector worldPos2 = fromPixelSpaceP(aPos2, mCameraPath);
            vector worldPos3 = fromPixelSpaceP(aPos3, mCameraPath);
            vector worldPos4 = fromPixelSpaceP(aPos4, mCameraPath);
            push(params, worldPos1.x); push(params, worldPos1.y); push(params, worldPos1.z);
            push(params, worldPos2.x); push(params, worldPos2.y); push(params, worldPos2.z);
            push(params, worldPos3.x); push(params, worldPos3.y); push(params, worldPos3.z);
            push(params, worldPos4.x); push(params, worldPos4.y); push(params, worldPos4.z);
        }
        else{
            push(params, aPos1.x); push(params, aPos1.y); push(params, aPos1.z);
            push(params, aPos2.x); push(params, aPos2.y); push(params, aPos2.z);
            push(params, aPos3.x); push(params, aPos3.y); push(params, aPos3.z);
            push(params, aPos4.x); push(params, aPos4.y); push(params, aPos4.z);
        }
        setattrib(geoself(), "point", "P5_mGeoType", pointNum, 0, "Quad", "set");
        setattrib(geoself(), "point", "P5_mGeoParams", pointNum, 0, params, "set");
        this -> transferAttribsToPoint(pointNum);
    }

    // MODES AND COLORS

    void background(vector4 aBackgroundColor)
    {
        // delete all points
        mShouldClearBuffer = 1;

        mBackgroundColor = aBackgroundColor;
    }

    void stroke(vector4 aStrokeColor)
    {
        mHasStroke = 1;
        mStrokeColor = aStrokeColor;
    }

    void fill(vector4 aFillColor)
    {
        mHasFill = 1;
        mFillColor = aFillColor;
    }

    void noStroke()
    {
        mHasStroke = 0;
    }

    void noFill()
    {
        mHasFill = 0;
    }

    void strokeWeight(float aStrokeWeight)
    {
        if (mShouldUsePixelSpace){
            mStrokeWeight = fromPixelSpaceS(aStrokeWeight, mCameraPath);
        }
        else{
            mStrokeWeight = aStrokeWeight;
        }
        
    }

    void sphereDetail(int aRes)
    {
        mSphereDetail = set(aRes, aRes);
    }

    void sphereDetail(int aURes, aVRes)
    {
        mSphereDetail = set(aURes, aVRes);
    }


    // MATRIX METHODS

    void translate(vector aTranslation)
    {
        vector inTranslation = aTranslation;
        if (mShouldUsePixelSpace){
            inTranslation = fromPixelSpaceD(inTranslation, mCameraPath);
        }

        matrix mat = this -> getCurrentMat();
        translate (mat, inTranslation);
        this -> setCurrentMat(mat);

        vector trans = this -> getCurrentTrans();
        trans += inTranslation;
        this -> setCurrentTrans(trans);
    }

    void scale(vector aScale)
    {
        vector inScale = aScale;

        matrix mat = this -> getCurrentMat();
        scale(mat, inScale);
        this -> setCurrentMat(mat);

        vector scale = this -> getCurrentScale();
        scale *= inScale;
        this -> setCurrentScale(scale);
    }

    void rotate(float aRadian)
    {
        matrix mat = this -> getCurrentMat();
        rotate(mat, aRadian, set(0.0, 0.0, 1.0));
        this -> setCurrentMat(mat);

        vector rot = this -> getCurrentRot();
        rot += set(0.0, 0.0, 1.0) * aRadian;
        this -> setCurrentRot(rot);
    }

    void rotateX(float aRadian)
    {
        matrix mat = this -> getCurrentMat();
        rotate(mat, aRadian, set(1.0, 0.0, 0.0));
        this -> setCurrentMat(mat);

        vector rot = this -> getCurrentRot();
        rot += set(1.0, 0.0, 0.0) * aRadian;
        this -> setCurrentRot(rot);
    }

    void rotateY(float aRadian)
    {
        matrix mat = this -> getCurrentMat();
        rotate(mat, aRadian, set(0.0, 1.0, 0.0));
        this -> setCurrentMat(mat);

        vector rot = this -> getCurrentRot();
        rot += set(0.0, 1.0, 0.0) * aRadian;
        this -> setCurrentRot(rot);
    }

    void rotateZ(float aRadian)
    {
        matrix mat = this -> getCurrentMat();
        rotate(mat, aRadian, set(0.0, 0.0, 1.0));
        this -> setCurrentMat(mat);

        vector rot = this -> getCurrentRot();
        rot += set(0.0, 0.0, 1.0) * aRadian;
        this -> setCurrentRot(rot);
    }

    void pushMatrix()
    {
        push(mMatrixList, ident());
        push(mTransList, set(0.0, 0.0, 0.0));
        push(mRotList, set(0.0, 0.0, 0.0));
        push(mScaleList, set(1.0, 1.0, 1.0));
    }

    void popMatrix()
    {
        mMatrixList = pop(mMatrixList);
        mTransList = pop(mTransList);
        mRotList = pop(mRotList);
        mScaleList = pop(mScaleList);
    }

    void resetMatrix()
    {
        matrix mat= ident();
        this -> setCurrentMat(mat);

        vector trans = set(0.0, 0.0, 0.0);
        this -> setCurrentTrans(trans);

        vector rot = set(0.0, 0.0, 0.0);
        this -> setCurrentRot(rot);

        vector scale = set(1.0, 1.0, 1.0);
        this -> setCurrentScale(scale);
    }

    // modes

    void strokeJoin(string aStrokeJoin)
    {
        if (aStrokeJoin != "MITER" && aStrokeJoin != "ROUND" && aStrokeJoin != "BEVEL") return;
        mStrokeJoin = aStrokeJoin;
    }

    void ellipseMode(string aEllipseMode)
    {
        if (aEllipseMode != "CENTER" && aEllipseMode != "RADIUS" && aEllipseMode != "CORNER" && aEllipseMode != "CORNERS") return;
        mEllipseMode = aEllipseMode;
    }

    void rectMode(string aRectMode)
    {
        if (aRectMode != "CENTER" && aRectMode != "RADIUS" && aRectMode != "CORNER" && aRectMode != "CORNERS") return;
        mRectMode = aRectMode;
    }
}

P5 openP5()
{
    P5 p;
    p -> open();
    return p;
}

#endif