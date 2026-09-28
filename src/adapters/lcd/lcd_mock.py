from enum import Enum, auto

class LCDMode(Enum):
    NONE = auto()
    SPLIT_IN_X = auto()
    SPLIT_IN_Y = auto()
    CIRCULAR = auto()
    DPC_PATTERN = auto()

class LCDController:
    def __init__(self, port=None):
        pass

    def configure(self):
        pass

    def update(self, mode, inner_radius, outer_radius, reverse, frame_count):
        pass

    def update_center(self, change_x, change_y):
        pass

    def close(self):
        pass