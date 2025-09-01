from fastapi import FastAPI
from pydantic import BaseModel
import joblib
import numpy as np

# Initialize FastAPI app
app = FastAPI(
    title="IoTricity Solar Tracker API",
    description="Predicts optimal solar panel angles using ML",
    version="1.0"
)

# Load trained models
mh = joblib.load("model_h.joblib")   # Horizontal model
mv = joblib.load("model_v.joblib")   # Vertical model

# Define input schema using Pydantic
class InputData(BaseModel):
    ldr_tl: float
    ldr_tr: float
    ldr_bl: float
    ldr_br: float
    horiz_pos: float
    vert_pos: float
    sun_az: float
    sun_el: float

# Root endpoint (health-check)
@app.get("/")
def read_root():
    return {"status": "ok", "message": "IoTricity API is running"}

# Prediction endpoint
@app.post("/predict")
def predict(data: InputData):
    # Convert input into NumPy array
    X = np.array([[data.ldr_tl, data.ldr_tr, data.ldr_bl, data.ldr_br,
                   data.horiz_pos, data.vert_pos, data.sun_az, data.sun_el]])

    # Get predictions
    pred_h = float(mh.predict(X)[0])
    pred_v = float(mv.predict(X)[0])

    return {
        "status": "success",
        "pred_horiz": round(pred_h, 2),
        "pred_vert": round(pred_v, 2),
        "note": "Angles are in degrees (0–180)"
    }
